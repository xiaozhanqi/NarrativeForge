#include "narrativeforge/api_key_manager.hpp"
#include <sqlite3.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <random>
#include <chrono>
#include <fstream>

namespace narrativeforge {

class APIKeyDatabase::Impl {
public:
    Impl(const std::string& db_path) : db_path_(db_path) {
        init_database();
        machine_id_ = generate_machine_id();
    }
    
    void init_database() {
        int rc = sqlite3_open(db_path_.c_str(), &db_);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Cannot open database");
        }
        
        const char* sql = R"(
            CREATE TABLE IF NOT EXISTS api_keys (
                id INTEGER PRIMARY KEY AUTOINCREMENT,
                provider TEXT UNIQUE NOT NULL,
                encrypted_key TEXT NOT NULL,
                model TEXT DEFAULT '',
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );
            
            CREATE TABLE IF NOT EXISTS machine_binding (
                id INTEGER PRIMARY KEY CHECK (id = 1),
                machine_id TEXT NOT NULL,
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );
        )";
        
        char* err_msg = nullptr;
        rc = sqlite3_exec(db_, sql, nullptr, nullptr, &err_msg);
        if (rc != SQLITE_OK) {
            std::string error = err_msg;
            sqlite3_free(err_msg);
            throw std::runtime_error("Failed to create tables: " + error);
        }
    }
    
    ~Impl() {
        if (db_) {
            sqlite3_close(db_);
        }
    }
    
    bool setup_api_key(const std::string& provider, const std::string& api_key, const std::string& model) {
        // Bind machine if not bound
        if (!is_machine_bound()) {
            bind_machine();
        }
        
        // Encrypt API key
        std::string encrypted = encrypt(api_key, machine_id_);
        
        // Store in database
        std::string sql = "INSERT OR REPLACE INTO api_keys (provider, encrypted_key, model, updated_at) "
                         "VALUES (?, ?, ?, CURRENT_TIMESTAMP)";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        sqlite3_bind_text(stmt, 1, provider.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, encrypted.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, model.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    std::optional<std::string> get_api_key(const std::string& provider) const {
        if (!is_machine_bound()) {
            throw std::runtime_error("Database is bound to different machine");
        }
        
        std::string sql = "SELECT encrypted_key FROM api_keys WHERE provider = ?";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return std::nullopt;
        
        sqlite3_bind_text(stmt, 1, provider.c_str(), -1, SQLITE_TRANSIENT);
        
        std::optional<std::string> result;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* encrypted = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            result = decrypt(encrypted, machine_id_);
        }
        
        sqlite3_finalize(stmt);
        return result;
    }
    
    bool has_api_key(const std::string& provider) const {
        auto key = get_api_key(provider);
        return key.has_value();
    }
    
    std::vector<APIKeyInfo> get_all_providers() const {
        std::vector<APIKeyInfo> result;
        
        std::string sql = "SELECT provider, model, created_at, updated_at FROM api_keys";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return result;
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            APIKeyInfo info;
            info.provider = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            info.model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            info.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            info.updated_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            result.push_back(info);
        }
        
        sqlite3_finalize(stmt);
        return result;
    }
    
    bool delete_api_key(const std::string& provider) {
        std::string sql = "DELETE FROM api_keys WHERE provider = ?";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        sqlite3_bind_text(stmt, 1, provider.c_str(), -1, SQLITE_TRANSIENT);
        
        rc = sqlite3_step(stmt);
        sqlite3_finalize(stmt);
        
        return rc == SQLITE_DONE;
    }
    
    bool is_machine_bound() const {
        std::string sql = "SELECT machine_id FROM machine_binding WHERE id = 1";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return false;
        
        bool bound = false;
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            const char* stored_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            bound = (stored_id == machine_id_);
        }
        
        sqlite3_finalize(stmt);
        return bound;
    }
    
    void bind_machine() {
        std::string sql = "INSERT OR REPLACE INTO machine_binding (id, machine_id) VALUES (1, ?)";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr);
        if (rc != SQLITE_OK) return;
        
        sqlite3_bind_text(stmt, 1, machine_id_.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    
    std::string get_machine_id() const {
        return machine_id_;
    }
    
    std::string encrypt(const std::string& data, const std::string& key) const {
        // Simple XOR encryption with base64 encoding
        std::string result;
        result.reserve(data.size());
        
        for (size_t i = 0; i < data.size(); ++i) {
            result += data[i] ^ key[i % key.size()];
        }
        
        // Base64 encode
        static const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        std::string encoded;
        int val = 0, valb = -6;
        
        for (unsigned char c : result) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                encoded.push_back(chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        
        if (valb > -6) {
            encoded.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
        }
        
        while (encoded.size() % 4) {
            encoded.push_back('=');
        }
        
        return encoded;
    }
    
    std::string decrypt(const std::string& encrypted_data, const std::string& key) const {
        // Base64 decode
        static const int table[] = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
            -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
            -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
        };
        
        std::string decoded;
        int val = 0, valb = -8;
        
        for (unsigned char c : encrypted_data) {
            if (table[c] == -1) break;
            val = (val << 6) + table[c];
            valb += 6;
            if (valb >= 0) {
                decoded.push_back(char((val >> valb) & 0xFF));
                valb -= 8;
            }
        }
        
        // XOR decrypt
        std::string result;
        result.reserve(decoded.size());
        
        for (size_t i = 0; i < decoded.size(); ++i) {
            result += decoded[i] ^ key[i % key.size()];
        }
        
        return result;
    }
    
    std::string generate_machine_id() const {
        // Generate unique machine identifier
        std::stringstream ss;
        
        // Use hostname
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == 0) {
            ss << hostname;
        }
        
        // Use random device
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 999999);
        ss << dis(gen);
        
        // Hash the result
        std::string combined = ss.str();
        unsigned char hash[32];
        
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
        EVP_DigestUpdate(ctx, combined.c_str(), combined.size());
        EVP_DigestFinal_ex(ctx, hash, nullptr);
        EVP_MD_CTX_free(ctx);
        
        // Convert to hex string
        std::stringstream hex_ss;
        for (int i = 0; i < 16; ++i) {
            hex_ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        
        return hex_ss.str();
    }
    
private:
    std::string db_path_;
    sqlite3* db_ = nullptr;
    std::string machine_id_;
};

APIKeyDatabase::APIKeyDatabase(const std::string& db_path)
    : pImpl_(std::make_unique<Impl>(db_path)) {}

APIKeyDatabase::~APIKeyDatabase() = default;

bool APIKeyDatabase::setup_api_key(const std::string& provider, 
                                   const std::string& api_key,
                                   const std::string& model) {
    return pImpl_->setup_api_key(provider, api_key, model);
}

std::optional<std::string> APIKeyDatabase::get_api_key(const std::string& provider) const {
    return pImpl_->get_api_key(provider);
}

bool APIKeyDatabase::has_api_key(const std::string& provider) const {
    return pImpl_->has_api_key(provider);
}

std::vector<APIKeyInfo> APIKeyDatabase::get_all_providers() const {
    return pImpl_->get_all_providers();
}

bool APIKeyDatabase::delete_api_key(const std::string& provider) {
    return pImpl_->delete_api_key(provider);
}

bool APIKeyDatabase::is_first_run() const {
    return !pImpl_->is_machine_bound();
}

bool APIKeyDatabase::is_machine_bound() const {
    return pImpl_->is_machine_bound();
}

std::string APIKeyDatabase::get_machine_id() const {
    return pImpl_->get_machine_id();
}

} // namespace narrativeforge
