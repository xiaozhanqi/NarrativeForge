from typing import List, Dict, Optional, Any, Union
from dataclasses import dataclass, field
import asyncio
import json
import numpy as np
from datetime import datetime
import re
from .api_router import UniversalAPIRouter
from .domain_adapter import DomainAdapter
from .database import NarrativeDatabase

@dataclass
class LearningConfig:
    id: str
    name: str
    provider: str
    model: str
    temperature: float
    top_p: float
    frequency_penalty: float
    presence_penalty: float = 0.0
    max_tokens: int
    chunk_strategy: str = "adaptive"  # adaptive/fixed/chapter_based
    chunk_size: int
    overlap: int = 0
    parallel_workers: int
    prompt_template: str
    output_schema: Union[Dict, str]
    input_from: Optional[List[str]] = None  # 依赖前序轮次ID
    domain_specific: bool = False
    extractors: List[str] = field(default_factory=list)

@dataclass
class ValidationConfig:
    id: str
    name: str
    provider: Optional[str]  # null表示本地验证
    validation_type: str  # temporal_spatial_consistency/character_psychology/fact_check/graph_consistency
    threshold: float
    reference_kb: Optional[str] = None
    ruleset: Optional[str] = None
    blocking: bool = True
    auto_fix: bool = False  # 失败时自动降低temperature重试

class ValidationError(Exception):
    pass

class CanonAnalysisEngine:
    def __init__(self, config_path: str, project_id: str):
        self.config = self._load_config(config_path)
        self.project_id = project_id
        self.state_db = NarrativeDatabase(f"projects/{project_id}/state.db")
        self.api_router = UniversalAPIRouter()
        self.domain_adapter = DomainAdapter(self.config.get("domain_knowledge", "generic"))
        
    def _load_config(self, config_path: str) -> Dict:
        """加载配置文件"""
        # 实际项目中应该从文件加载
        return {
            "domain_knowledge": "generic_fantasy",
            "learning_rounds": [
                {
                    "id": "L1_SemanticChunking",
                    "name": "语义分块与实体识别",
                    "provider": "kimi",
                    "model": "kimi-k1.5",
                    "temperature": 0.1,
                    "top_p": 0.95,
                    "frequency_penalty": 0.0,
                    "max_tokens": 4096,
                    "chunk_strategy": "adaptive",
                    "chunk_size": 6000,
                    "overlap": 200,
                    "parallel_workers": 4,
                    "prompt_template": "prompts/analysis/entity_extraction.txt",
                    "output_schema": {},
                    "extractors": ["character", "location", "item", "timeline"]
                },
                {
                    "id": "L2_RelationalTopology",
                    "name": "关系拓扑构建",
                    "provider": "lmstudio",
                    "model": "deepseek-v3-q5",
                    "temperature": 0.0,
                    "top_p": 0.9,
                    "frequency_penalty": 0.0,
                    "max_tokens": 8192,
                    "context_window": 128000,
                    "input_from": ["L1_SemanticChunking"],
                    "output_schema": {},
                    "graph_type": "dynamic"
                },
                {
                    "id": "L3_WorldbuildingCanon",
                    "name": "世界观规则提取",
                    "provider": "volcano",
                    "model": "doubao-pro-128k",
                    "temperature": 0.1,
                    "top_p": 0.9,
                    "frequency_penalty": 0.0,
                    "max_tokens": 4096,
                    "domain_specific": True,
                    "knowledge_base": "kb/magic_systems.db",
                    "extract_rules": ["physical_laws", "social_structure", "economic_system", "magic_rules"]
                },
                {
                    "id": "L4_ForeshadowingMining",
                    "name": "伏笔与线索挖掘",
                    "provider": "kimi",
                    "model": "kimi-k1.5-long-context",
                    "temperature": 0.2,
                    "top_p": 0.9,
                    "frequency_penalty": 0.0,
                    "max_tokens": 8192,
                    "pattern_matching": ["chekhov_gun", "red_herring", "parallel_plot", "symbolism"]
                }
            ],
            "validation_rounds": [
                {
                    "id": "V1_ConsistencyCheck",
                    "name": "时空一致性校验",
                    "provider": "local",
                    "validation_type": "temporal_spatial_consistency",
                    "threshold": 0.9,
                    "ruleset": "rules/timeline_checker.py"
                },
                {
                    "id": "V2_CharacterCoherence",
                    "name": "人物行为逻辑校验",
                    "provider": "volcano",
                    "validation_type": "character_psychology",
                    "threshold": 0.90,
                    "reference_kb": "kb/personality_models.db",
                    "blocking": True
                },
                {
                    "id": "V3_DomainAccuracy",
                    "name": "领域知识准确性",
                    "provider": "multi",
                    "validation_type": "fact_check",
                    "threshold": 0.95,
                    "domain": "generic_fantasy"
                }
            ]
        }
    
    def _chunk_text(self, text: str, strategy: str, size: int, overlap: int) -> List[str]:
        """智能分块策略"""
        if strategy == "chapter_based":
            # 基于章节标题正则分割
            chapters = re.split(r'(第[一二三四五六七八九十百零]+章|\\bChapter\\s+\\d+)', text)
            return [chapters[i] + chapters[i+1] for i in range(1, len(chapters), 2) if i+1 < len(chapters)]
        elif strategy == "scene_based":
            # 基于场景转换（空行、时间跳跃词）
            scenes = re.split(r'\n\s*\n', text)
            return scenes
        else:  # adaptive
            # 语义分块：在句子边界处切割，保持上下文连贯
            chunks = []
            start = 0
            while start < len(text):
                end = min(start + size, len(text))
                if end < len(text):
                    # 寻找最近的自然断句
                    while end > start and text[end] not in '.。!！?？\n':
                        end -= 1
                    end += 1
                chunks.append(text[start:end])
                start = end - overlap  # 保留重叠区域
            return chunks
    
    def _load_template(self, template_path: str) -> str:
        """加载提示模板"""
        # 实际项目中应该从文件加载
        return "请分析以下文本，提取{{extractors}}：\n\n{{chunk}}"
    
    def _merge_structured_results(self, results: List[Dict], schema: Dict) -> Dict:
        """合并结构化结果"""
        # 实际项目中应该根据schema合并结果
        return {"merged": results}
    
    def _find_matching_validation(self, round_id: str) -> Optional[Dict]:
        """找到匹配的验证轮"""
        # 实际项目中应该根据配置找到匹配的验证轮
        return None
    
    def _load_local_validator(self, validation_type: str, ruleset: str):
        """加载本地验证器"""
        # 实际项目中应该从文件加载验证器
        class DummyValidator:
            def check(self, input_data, reference_data):
                return {"score": 0.95, "issues": []}
        return DummyValidator()
    
    async def _multi_model_consensus(self, config: Dict, input_data: Dict) -> Dict:
        """多模型共识验证"""
        # 实际项目中应该调用多个模型进行验证
        return {"score": 0.96, "issues": []}
    
    def _build_validation_prompt(self, config: ValidationConfig, input_data: Dict, reference_data: Optional[Dict]) -> str:
        """构建验证提示"""
        return "请验证以下内容的准确性：\n\n" + json.dumps(input_data)
    
    async def _generate_fix_suggestion(self, config: ValidationConfig, issues: List) -> str:
        """生成修复建议"""
        return "建议：检查时空一致性"
    
    def _generate_author_embedding(self, results: Dict) -> List[float]:
        """生成作者风格向量"""
        # 实际项目中应该基于分析结果生成向量
        return [0.0] * 768
    
    async def execute_learning(self, round_id: str, source_text: str, 
                               previous_results: Dict[str, Any] = None) -> Dict:
        """Execute single learning round"""
        cfg = next(r for r in self.config['learning_rounds'] if r['id'] == round_id)
        config = LearningConfig(**cfg)
        
        # 1. 智能分块
        chunks = self._chunk_text(source_text, config.chunk_strategy, 
                                config.chunk_size, config.overlap)
        
        # 2. 构建增强提示（注入Domain Knowledge）
        base_template = self._load_template(config.prompt_template)
        domain_context = self.domain_adapter.get_context(config.extractors)
        
        tasks = []
        for idx, chunk in enumerate(chunks):
            prompt = base_template.format(
                chunk=chunk,
                chunk_idx=idx,
                total_chunks=len(chunks),
                domain_context=domain_context,
                previous_analysis=json.dumps(previous_results.get(config.input_from[-1], {})) if config.input_from else "{}"
            )
            
            task = {
                'provider': config.provider,
                'model': config.model,
                'messages': [{'role': 'user', 'content': prompt}],
                'temperature': config.temperature,
                'top_p': config.top_p,
                'frequency_penalty': config.frequency_penalty,
                'presence_penalty': config.presence_penalty,
                'max_tokens': config.max_tokens
            }
            tasks.append(task)
        
        # 3. 并发执行与进度回调
        results = await self.api_router.batch_call(
            tasks,
            max_concurrent=config.parallel_workers,
            progress_callback=lambda cur, tot: print(f"{round_id}: {cur}/{tot}")
        )
        
        # 4. 结果合并与结构化
        merged = self._merge_structured_results(results, config.output_schema)
        
        # 5. 持久化
        self.state_db.save_learning_round(self.project_id, round_id, config, merged)
        
        return merged
    
    async def execute_validation(self, round_id: str, input_data: Dict, 
                                 reference_data: Optional[Dict] = None) -> Dict:
        """执行验证轮"""
        cfg = next(r for r in self.config['validation_rounds'] if r['id'] == round_id)
        config = ValidationConfig(**cfg)
        
        if config.provider == 'local':
            # 本地规则引擎验证（零成本）
            validator = self._load_local_validator(config.validation_type, config.ruleset)
            result = validator.check(input_data, reference_data)
        elif config.provider == 'multi':
            # 多模型共识验证（对抗性）
            result = await self._multi_model_consensus(cfg, input_data)
        else:
            # API远程验证
            prompt = self._build_validation_prompt(config, input_data, reference_data)
            api_result = await self.api_router.call_single(
                provider=config.provider,
                model=config.model,
                messages=[{'role': 'user', 'content': prompt}]
            )
            result = json.loads(api_result)
        
        passed = result['score'] >= config.threshold
        
        # 自动修复逻辑
        if not passed and config.auto_fix:
            result['fix_suggestion'] = await self._generate_fix_suggestion(config, result['issues'])
            
        self.state_db.save_validation_round(self.project_id, round_id, passed, result)
        
        return {
            'round_id': round_id,
            'passed': passed,
            'score': result['score'],
            'issues': result.get('issues', []),
            'timestamp': datetime.now().isoformat()
        }
    
    async def run_analysis_pipeline(self, source_text: str, mode: str = 'full') -> Dict:
        """运行完整分析流水线"""
        results = {}
        
        # 顺序执行Learning（有依赖关系）
        for cfg in self.config['learning_rounds']:
            round_id = cfg['id']
            
            if self.state_db.is_learning_completed(self.project_id, round_id) and mode != 'force_restart':
                results[round_id] = self.state_db.load_learning_result(self.project_id, round_id)
                continue
            
            print(f"[Analysis] Starting {cfg['name']}...")
            
            # 收集依赖数据
            prev_results = {}
            if cfg.get('input_from'):
                for dep_id in cfg['input_from']:
                    prev_results[dep_id] = results.get(dep_id) or self.state_db.load_learning_result(self.project_id, dep_id)
            
            data = await self.execute_learning(round_id, source_text, prev_results)
            results[round_id] = data
            
            # 立即执行关联验证
            v_cfg = self._find_matching_validation(round_id)
            if v_cfg:
                v_result = await self.execute_validation(v_cfg['id'], data, prev_results)
                if not v_result['passed'] and v_cfg.get('blocking', True):
                    if v_cfg.get('auto_fix'):
                        # 自动修复模式：调整参数重新分析
                        print(f"Validation failed, attempting auto-fix...")
                        # 实现修复逻辑
                    else:
                        raise ValidationError(f"{round_id} validation failed: {v_result['issues']}")
        
        # 编译叙事张量（Narrative Tensor）
        narrative_tensor = self._compile_narrative_tensor(results)
        self.state_db.save_project_metadata(self.project_id, 'narrative_tensor', narrative_tensor)
        
        return narrative_tensor
    
    def _compile_narrative_tensor(self, results: Dict) -> Dict:
        """将多轮结果编译为高密度叙事张量（768d向量 + 结构化知识）"""
        # 融合实体、关系、世界观、伏笔为统一表示
        tensor = {
            'entity_space': results.get('L1_SemanticChunking', {}),
            'relation_manifold': results.get('L2_RelationalTopology', {}),
            'world_rules': results.get('L3_WorldbuildingCanon', {}),
            'foreshadowing_field': results.get('L4_ForeshadowingMining', {}),
            'embedding_vector': self._generate_author_embedding(results),  # 作者风格向量
            'version': '1.0',
            'compiled_at': datetime.now().isoformat()
        }
        return tensor
