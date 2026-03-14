# NarrativeForge

<div align="center">

![Version](https://img.shields.io/badge/version-0.4.3-blue.svg)
![Stage](https://img.shields.io/badge/stage-beta-orange.svg)
![C++](https://img.shields.io/badge/C++-17-green.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)
![Status](https://img.shields.io/badge/status-active-success.svg)

**Universal Narrative Engineering Platform**

A general-purpose narrative engine for analyzing and generating long-form text using advanced AI technologies.

[Features](#-features) • [Installation](#-installation) • [Usage](#-usage) • [Documentation](#-documentation)

</div>

---

## 📖 About

NarrativeForge is a sophisticated narrative engineering platform designed to analyze and generate long-form text content. It leverages cutting-edge AI technologies to provide comprehensive text analysis and intelligent content generation capabilities.

### 🎯 Development Status

- **Status**: Active Development
- **Version**: 0.4.3 (beta)
- **Implementation**: C++17
- **AI Technology**: Utilizes advanced Large Language Models (LLMs) including Kimi, Volcano, and OpenAI
- **Core Innovation**: Proprietary Narrative Tensor representation and Quantum World State management
- **Platform Support**: Cross-platform (Windows, macOS, Linux)

### 🎯 Key Features

- **Dual-Core Architecture**: Separate analysis and generation engines with decoupled pipelines
- **Narrative Tensor**: 768-dimensional vector + structured knowledge representation
- **Quantum World State**: Advanced state management tracking entities, timeline, and foreshadowing
- **L/V Rounds**: Learning and validation rounds for both analysis and generation
- **Domain Adaptation**: Support for multiple genres (scifi, fantasy, historical, mystery)
- **Multiple Interfaces**: CLI, Qt6 GUI, and Web Server
- **Modular Design**: Unified controller with independent functional modules
- **Encrypted Storage**: One machine one code API key storage

## 🚀 Features

### Analysis Engine
- **L1 - Semantic Chunking**: Intelligent text segmentation and entity recognition
- **L2 - Relational Topology**: Building character and event relationships
- **L3 - Worldbuilding Canon**: Extracting world rules and constraints
- **L4 - Foreshadowing Mining**: Identifying narrative clues and setup
- **Validation Rounds**: Temporal consistency, character coherence, domain accuracy

### Generation Engine
- **L1 - Plot Topology**: Constructing narrative structure
- **L2 - World State Init**: Quantum state initialization
- **L3 - Scene Drafting**: Parallel scene generation (3 versions)
- **L4 - Dialogue Sculpting**: Character voice locking and refinement
- **L5 - Prose Polishing**: Style transfer and literary enhancement
- **Validation Rounds**: OOC check, canon compliance, narrative coherence

### Generation Modes
- **Continuation**: Continue existing stories
- **AU (Alternative Universe)**: Create alternate scenarios
- **Cross-Genre**: Adapt stories to different genres
- **Interactive**: Generate interactive fiction experiences

### AI Provider Support
- **Kimi**: Advanced language model integration
- **Volcano**: High-performance inference
- **OpenAI**: GPT models support
- **LM Studio**: Local model support

## 📦 Installation

### Prerequisites
- C++17 or higher
- CMake 3.16+
- Qt6 (for GUI)
- OpenSSL
- SQLite3
- libcurl
- nlohmann/json

### Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/xiaozhanqi/narrativeforge.git
   cd narrativeforge
   ```

2. **Build on Linux/macOS**
   ```bash
   cd cpp
   mkdir build && cd build
   cmake ..
   make -j$(nproc)
   ```

3. **Build on Windows**
   ```powershell
   cd cpp
   mkdir build
   cd build
   cmake ..
   cmake --build . --config Release
   ```

### Dependencies Installation

**Ubuntu/Debian:**
```bash
sudo apt-get install cmake libssl-dev libsqlite3-dev libcurl4-openssl-dev nlohmann-json3-dev qt6-base-dev
```

**macOS:**
```bash
brew install cmake openssl sqlite curl nlohmann-json qt6
```

**Windows (vcpkg):**
```powershell
vcpkg install openssl sqlite3 curl nlohmann-json qt6-base
```

## 🎮 Usage

### CLI Mode

Launch command-line interface:

```bash
./build/narrativeforge_cli
```

**Features:**
- Interactive menu system
- Text analysis and generation
- API key management
- Progress tracking

### GUI Mode

Launch Qt6 desktop application:

```bash
./build/narrativeforge_gui
```

**Features:**
- Intuitive drag-and-drop interface
- Real-time progress tracking
- Visual workspace switching
- Export functionality for tensors and generated text

### Web Server Mode

Start web server:

```bash
./build/narrativeforge_web
```

Then open your browser and navigate to: **http://localhost:5000**

**Features:**
- Modern responsive design
- Cross-platform compatibility
- RESTful API integration
- Real-time status updates

## 📁 Project Structure

```
narrativeforge/
├── cpp/                          # C++ Implementation
│   ├── include/narrativeforge/   # Header files
│   │   ├── core.hpp             # Core engine
│   │   ├── api_router.hpp       # API router
│   │   ├── api_key_manager.hpp  # API key manager
│   │   ├── module_interface.hpp # Module interface
│   │   └── app_controller.hpp   # Main controller
│   ├── src/                     # Source files
│   │   ├── core/                # Core implementation
│   │   │   ├── analyzer.cpp
│   │   │   ├── generator.cpp
│   │   │   ├── api_router.cpp
│   │   │   ├── api_key_manager.cpp
│   │   │   └── app_controller.cpp
│   │   ├── main.cpp             # Main entry
│   │   └── main_cli.cpp         # CLI program
│   ├── CMakeLists.txt           # Build configuration
│   └── README.md                # C++ documentation
├── config/                       # Configuration files
│   ├── analyzer_schema.json
│   └── generator_schema.json
├── templates/                    # Web templates
│   └── index.html
├── sample_canon.txt             # Sample text
└── README.md                    # This file
```

## ⚙️ Configuration

### Analysis Engine Configuration

The analysis engine supports multiple domains:

- **generic**: General purpose text analysis
- **scifi**: Hard science fiction specialization
- **fantasy**: Fantasy genre adaptation
- **historical**: Historical fiction context
- **mystery**: Mystery and suspense focus

### Generation Engine Configuration

The generation engine supports multiple modes:

- **continuation**: Continue original story
- **au**: Alternative Universe scenarios
- **cross_genre**: Cross-genre adaptation
- **interactive**: Interactive fiction generation

### AI Provider Configuration

Configure AI providers in `config/analyzer_schema.json` and `config/generator_schema.json`:

- **kimi**: Moonshot AI language model
- **volcano**: ByteDance inference platform
- **openai**: OpenAI GPT models
- **lmstudio**: Local model management

## � Development

### Building Tests

```bash
cmake -DBUILD_TESTS=ON ..
make
ctest
```

### Adding New Features

1. **Core Logic**: Add to `cpp/src/core/` modules
2. **UI Components**: Add to `cpp/src/ui/` or `templates/`
3. **Configuration**: Update `config/` JSON files
4. **Documentation**: Update this README file

### Code Style

- Follow C++17 standards
- Use modern C++ features
- RAII for resource management
- Smart pointers for memory safety
- Use `#pragma once` for header guards

## 📊 Examples

### Basic Analysis (C++)

```cpp
#include "narrativeforge/app_controller.hpp"

using namespace narrativeforge;

int main() {
    auto& app = AppController::instance();
    app.initialize();
    
    std::string text = "Your text here...";
    auto future = app.analyze_text(text);
    NarrativeTensor tensor = future.get();
    
    // Save tensor
    std::ofstream out("tensor.json");
    out << tensor.to_json().dump(2);
    
    return 0;
}
```

### Basic Generation (C++)

```cpp
#include "narrativeforge/app_controller.hpp"
#include <fstream>

using namespace narrativeforge;

int main() {
    // Load tensor
    std::ifstream file("tensor.json");
    nlohmann::json j;
    file >> j;
    NarrativeTensor tensor = NarrativeTensor::from_json(j);
    
    auto& app = AppController::instance();
    app.initialize();
    
    auto future = app.generate_text(tensor, "continuation");
    std::string text = future.get();
    
    std::cout << text << std::endl;
    
    return 0;
}
```

## 🐛 Troubleshooting

### Build Errors

**CMake not found:**
- Install CMake 3.16 or higher

**OpenSSL not found:**
- Install OpenSSL development libraries
- Set `OPENSSL_ROOT_DIR` environment variable

**Qt6 not found:**
- Install Qt6 and set `Qt6_DIR` environment variable
- Example: `export Qt6_DIR=/path/to/Qt6/lib/cmake/Qt6`

### Runtime Errors

**API key not configured:**
- Run the setup wizard on first run
- Or manually configure API keys in the database

**Database errors:**
- Check file permissions
- Ensure data directory exists

**Module errors:**
- Check module status with option 6 in CLI
- Review error messages in console

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Development Setup

```bash
# Clone repository
git clone https://github.com/xiaozhanqi/narrativeforge.git
cd narrativeforge

# Create build directory
cd cpp
mkdir build && cd build

# Configure and build
cmake ..
make -j$(nproc)

# Run tests
ctest
```

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- **Moonshot AI** for the Kimi language model
- **ByteDance** for the Volcano platform
- **Qt Company** for the excellent Qt framework
- **nlohmann** for the JSON library

## 📞 Support

- **Issues**: Report bugs and request features via [GitHub Issues](https://github.com/xiaozhanqi/narrativeforge/issues)
- **Discussions**: Join our community discussions
- **Documentation**: Check out our [Wiki](https://github.com/xiaozhanqi/narrativeforge/wiki)

## 🔗 Links

- [Documentation](https://github.com/xiaozhanqi/narrativeforge/wiki)
- [API Reference](https://github.com/xiaozhanqi/narrativeforge/blob/main/docs/API.md)
- [Examples](https://github.com/xiaozhanqi/narrativeforge/tree/main/examples)
- [Changelog](https://github.com/xiaozhanqi/narrativeforge/blob/main/CHANGELOG.md)

### Development Notice
- **AI-Assisted Development**: Portions of this codebase were developed with AI assistance.
- **Beta Status**: This software is in early beta and has not undergone extensive production testing. Stability cannot be guaranteed.
- **Recommendation**: We recommend testing in an isolated environment or virtual machine before production deployment.


## Disclaimer

**Last Updated:** March 13, 2026

### 1. Software Provision
NarrativeForge is provided **"AS IS"** and **"AS AVAILABLE"** without any warranties of any kind, either express or implied, including but not limited to the implied warranties of merchantability, fitness for a particular purpose, or non-infringement. The entire risk as to the quality and performance of the software is with you.

### 2. AI-Generated Content
This software utilizes Large Language Models (LLMs) including but not limited to Moonshot Kimi, ByteDance Volcano Engine, and local models to generate text content. 
- The developers of NarrativeForge assume **no responsibility** for the accuracy, appropriateness, or legality of AI-generated outputs.
- Users are solely responsible for reviewing, editing, and ensuring compliance with applicable laws and copyright regulations regarding generated content.
- Generated content does not represent the views or opinions of the NarrativeForge development team.

### 3. Data Privacy & Security
- **Local Processing**: Analysis and generation primarily occur via user-configured API keys or local models. NarrativeForge does not collect, store, or transmit user data to centralized servers controlled by the developers.
- **Third-Party Services**: When using external AI providers (Kimi, Volcano, etc.), data transmission is subject to their respective Terms of Service and Privacy Policies. Users are responsible for reviewing these terms before submitting sensitive or proprietary content.
- **No Warranty for Data Loss**: The developers are not liable for any data loss, corruption, or unauthorized access resulting from the use of this software.

### 4. Third-Party Services
NarrativeForge integrates with third-party AI platforms. We do not guarantee the availability, accuracy, or continuity of these services. Service interruptions, API changes, or termination by third-party providers are beyond our control.

### 5. Absolute Limitation of Liability & Risk Allocation

**BY USING NARRATIVEFORGE, YOU EXPRESSLY ACKNOWLEDGE THAT THE ENTIRE RISK OF USE REMAINS WITH YOU. UNDER NO CIRCUMSTANCES SHALL THE DEVELOPERS BE LIABLE TO YOU OR ANY THIRD PARTY FOR ANY REASON WHATSOEVER.**

#### 5.1 Absolute Exclusion of All Liability
To the maximum extent permitted by applicable law, the authors, copyright holders, contributors, distributors, and any affiliated parties of NarrativeForge **ASSUME NO LIABILITY** for any damages, losses, costs, expenses, or harms of any nature, whether known or unknown, foreseen or unforeseen, including but not limited to:

- Direct, indirect, incidental, special, consequential, exemplary, or punitive damages
- Loss of profits, revenue, business, goodwill, data, content, or anticipated savings
- Personal injury, reputational harm, or emotional distress
- System damage, data corruption, security breaches, or malware infections
- Claims by third parties for intellectual property infringement, defamation, privacy violations, or harmful content
- Regulatory fines, legal fees, or compliance costs
- Damages arising from AI hallucinations, biases, errors, or inappropriate outputs

**THE FOREGOING LIMITATION APPLIES REGARDLESS OF THE FORM OF ACTION, WHETHER IN CONTRACT, TORT (INCLUDING NEGLIGENCE), STRICT LIABILITY, BREACH OF WARRANTY, OR OTHERWISE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.**

#### 5.2 No Duty of Care & No Reliance
- **No Professional Duty**: NarrativeForge is provided as experimental software, not as a professional service. No fiduciary, professional, or advisory relationship is created between you and the developers.
- **No Reliance Permitted**: You acknowledge that any reliance on AI-generated analysis, content, or outputs is at your own risk. Developers make no representations about accuracy, completeness, or fitness for any purpose.
- **Verification Required**: You are solely responsible for independently verifying all outputs before use, publication, or commercial application.

#### 5.3 Complete User Assumption of Risk & Responsibility
You expressly agree that:
- **Full Responsibility**: You bear **100% of legal, financial, and ethical responsibility** for all inputs provided to the system and all outputs generated by the system.
- **No Developer Control**: Developers have no control over, knowledge of, or ability to monitor how you use the software or the content you create with it.
- **Independent Action**: Your use of NarrativeForge constitutes an independent act for which you alone are answerable to third parties, regulatory authorities, or courts of law.

#### 5.4 No Vicarious or Derivative Liability
Developers shall not be held liable for:
- **User Misconduct**: Any unlawful, unethical, or harmful acts committed by you using the software
- **Third-Party Claims**: Any claims brought against you by third parties arising from your use of AI-generated content
- **Downstream Consequences**: Any damages resulting from your publication, distribution, or commercialization of generated content
- **Chain of Liability**: Developers are not part of any chain of liability for content created by users; users are the sole originators and publishers of such content.

#### 5.5 No Agency, Partnership, or Joint Venture
Nothing in this disclaimer or your use of the software shall be construed as creating:
- An agency relationship where developers act on your behalf
- A partnership or joint venture for content creation
- Any form of principal-agent relationship that could impute user conduct to developers

You act wholly on your own behalf and for your own account. Developers remain passive providers of software tools only.

#### 5.6 Class Action & Representative Action Waiver
You agree to waive any right to:
- Participate as a plaintiff or class member in any purported class action, collective action, or representative proceeding against the developers
- Seek damages or equitable relief on behalf of a group of users
- Consolidate your claims with claims of other users

All disputes, if any (which are disclaimed above), must be brought solely in your individual capacity.

#### 5.7 Liability Cap (If Any Liability Found Despite This Disclaimer)
**IF, NOTWITHSTANDING THE FOREGOING EXCLUSIONS, A COURT OF COMPETENT JURISDICTION DETERMINES THAT DEVELOPERS HAVE ANY LIABILITY TO YOU, SUCH LIABILITY SHALL BE STRICTLY LIMITED TO THE LESSER OF:**
- **USD $1.00 (One United States Dollar), or**
This cap applies regardless of the number of claims, theories of liability, or nature of damages alleged.

#### 5.8 Covenant Not to Sue & Release
To the fullest extent permitted by law, you hereby:
- **Covenant Not to Sue**: Agree not to initiate, assert, or maintain any legal action, lawsuit, or proceeding of any kind against the developers based on any theory of liability arising from or related to the software
- **General Release**: Release the developers from all claims, demands, and causes of action, known or unknown, arising from your use of NarrativeForge or any content generated thereby

#### 5.9 Severability & Survival
If any portion of this limitation of liability is held invalid or unenforceable in any jurisdiction:
- That portion shall be interpreted to reflect the parties' original intent as closely as possible
- The remaining portions shall remain in full force and effect
- These limitations shall survive any termination of your use of the software and continue indefinitely

#### 5.10 Users Subject to Enhanced Scrutiny
If you are a business, corporation, or commercial entity using NarrativeForge:
- You represent that you have independent legal counsel
- You acknowledge that this is a business-to-business software provision
- You confirm that you have the capacity to assume and manage all risks associated with AI-generated content
- You agree that consumer protection laws do not apply to your use of this software

### 6. Usage Responsibility
By using NarrativeForge, you agree that:
- You will not use the software for illegal activities, harassment, generating harmful content, or violating intellectual property rights.
- You assume full responsibility for the input data provided to the system and the generated outputs.
- You will comply with all applicable local, national, and international laws regarding AI-generated content.

### 7. Beta Software Notice
NarrativeForge is currently in beta (v0.4.3). Features may change without notice, and backward compatibility is not guaranteed. The API, database schema, and configuration formats are subject to breaking changes during the development phase.

### 8. Educational and Research Use
This software is intended for narrative analysis research, creative writing assistance, and educational purposes. It is not intended as a substitute for professional editorial services, legal advice, or critical decision-making.


---

<div align="center">

**Built with ❤️ by the NarrativeForge Team**

[⬆ Back to Top](#narrativeforge)

</div>
