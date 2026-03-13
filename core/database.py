import sqlite3
import json
import zlib
from datetime import datetime
from typing import Dict, Any, Optional

class NarrativeDatabase:
    SCHEMA = """
    CREATE TABLE IF NOT EXISTS projects (
        project_id TEXT PRIMARY KEY,
        created_at TIMESTAMP,
        canon_text_path TEXT,
        domain TEXT,
        status TEXT
    );
    
    CREATE TABLE IF NOT EXISTS learning_rounds (
        id TEXT,
        project_id TEXT,
        round_name TEXT,
        config_json TEXT,  -- 配置快照
        output_data BLOB,  -- zlib压缩的JSON
        status TEXT,
        started_at TIMESTAMP,
        completed_at TIMESTAMP,
        PRIMARY KEY (id, project_id)
    );
    
    CREATE TABLE IF NOT EXISTS validation_rounds (
        id TEXT,
        project_id TEXT,
        ref_learning_round TEXT,
        passed BOOLEAN,
        score REAL,
        issues_json TEXT,
        validated_at TIMESTAMP,
        PRIMARY KEY (id, project_id)
    );
    
    CREATE TABLE IF NOT EXISTS world_states (
        project_id TEXT,
        iteration INTEGER,
        state_hash TEXT,
        state_json BLOB,  -- 压缩存储
        checkpoint_name TEXT,
        created_at TIMESTAMP,
        PRIMARY KEY (project_id, iteration)
    );
    
    CREATE TABLE IF NOT EXISTS api_logs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        project_id TEXT,
        timestamp TIMESTAMP,
        provider TEXT,
        model TEXT,
        round_id TEXT,
        tokens_input INTEGER,
        tokens_output INTEGER,
        cost_estimate REAL,  -- 预估成本（USD）
        latency_ms INTEGER,
        status_code INTEGER
    );
    """
    
    def __init__(self, db_path: str):
        self.db_path = db_path
        self.conn = sqlite3.connect(db_path)
        self._init_schema()
    
    def _init_schema(self):
        """初始化数据库 schema"""
        cursor = self.conn.cursor()
        cursor.executescript(self.SCHEMA)
        self.conn.commit()
    
    def save_project(self, project_id: str, canon_text_path: str, domain: str):
        """保存项目信息"""
        cursor = self.conn.cursor()
        cursor.execute(
            "INSERT OR REPLACE INTO projects VALUES (?, ?, ?, ?, ?)",
            (project_id, datetime.now().isoformat(), canon_text_path, domain, 'active')
        )
        self.conn.commit()
    
    def save_learning_round(self, project_id: str, round_id: str, config: Any, data: Dict):
        """保存学习轮结果"""
        compressed = zlib.compress(json.dumps(data, ensure_ascii=False).encode())
        cursor = self.conn.cursor()
        cursor.execute(
            "INSERT OR REPLACE INTO learning_rounds VALUES (?, ?, ?, ?, ?, ?, ?, ?)",
            (round_id, project_id, config.name, json.dumps(config.__dict__), compressed, 
             'completed', datetime.now().isoformat(), datetime.now().isoformat())
        )
        self.conn.commit()
    
    def save_validation_round(self, project_id: str, round_id: str, passed: bool, result: Dict):
        """保存验证轮结果"""
        cursor = self.conn.cursor()
        cursor.execute(
            "INSERT OR REPLACE INTO validation_rounds VALUES (?, ?, ?, ?, ?, ?, ?)",
            (round_id, project_id, None, passed, result.get('score', 0), 
             json.dumps(result.get('issues', [])), datetime.now().isoformat())
        )
        self.conn.commit()
    
    def save_world_state(self, project_id: str, iteration: int, state_hash: str, state: Dict, checkpoint_name: str = None):
        """保存世界状态"""
        compressed = zlib.compress(json.dumps(state, ensure_ascii=False).encode())
        cursor = self.conn.cursor()
        cursor.execute(
            "INSERT OR REPLACE INTO world_states VALUES (?, ?, ?, ?, ?, ?)",
            (project_id, iteration, state_hash, compressed, checkpoint_name, datetime.now().isoformat())
        )
        self.conn.commit()
    
    def load_learning_result(self, project_id: str, round_id: str) -> Optional[Dict]:
        """加载学习轮结果"""
        cursor = self.conn.cursor()
        cursor.execute(
            "SELECT output_data FROM learning_rounds WHERE project_id = ? AND id = ?",
            (project_id, round_id)
        )
        result = cursor.fetchone()
        if result:
            return json.loads(zlib.decompress(result[0]).decode())
        return None
    
    def is_learning_completed(self, project_id: str, round_id: str) -> bool:
        """检查学习轮是否完成"""
        cursor = self.conn.cursor()
        cursor.execute(
            "SELECT status FROM learning_rounds WHERE project_id = ? AND id = ?",
            (project_id, round_id)
        )
        result = cursor.fetchone()
        return result is not None and result[0] == 'completed'
    
    def save_project_metadata(self, project_id: str, key: str, value: Any):
        """保存项目元数据"""
        # 实际项目中应该有专门的元数据表
        pass
