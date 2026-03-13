from typing import List, Dict, Optional, Any
import numpy as np
import json
from datetime import datetime
import asyncio
from .api_router import UniversalAPIRouter
from .database import NarrativeDatabase

class QuantumWorldState:
    """量子态世界状态管理器（保留原设计的坍缩机制）"""
    def __init__(self, initial_entities: Dict, foreclosure_list: Dict):
        self.state = {
            'entities': initial_entities,  # 人物、物品、地点的当前状态
            'timeline': [],  # 已发生事件序列
            'active_foreshadowings': foreclosure_list,  # 待回收伏笔
            'resolved_foreshadowings': [],
            'global_variables': {}  # 世界观变量（如魔法能量水平、社会动荡指数）
        }
        self.history_hash = self._compute_hash()
    
    def _compute_hash(self) -> str:
        """计算世界状态哈希"""
        return str(hash(json.dumps(self.state, sort_keys=True)))
    
    def update_from_generation(self, round_id: str, generated_content: Dict):
        """根据生成内容坍缩世界状态"""
        if round_id == 'L3_SceneDrafting':
            # 更新情节发展
            new_events = generated_content.get('events', [])
            self.state['timeline'].extend(new_events)
            
            # 更新实体状态（位置、持有物品、心理状态）
            for entity_update in generated_content.get('entity_updates', []):
                entity_id = entity_update['id']
                if entity_id in self.state['entities']:
                    self.state['entities'][entity_id].update(entity_update['state'])
            
            # 检查伏笔回收
            resolved = []
            for fs in self.state['active_foreshadowings']:
                if self._check_foreclosure_resolved(fs, new_events):
                    resolved.append(fs)
            for r in resolved:
                self.state['active_foreshadowings'].remove(r)
                self.state['resolved_foreshadowings'].append(r)
                
        self.history_hash = self._compute_hash()
    
    def _check_foreclosure_resolved(self, foreshadowing: Dict, events: List) -> bool:
        """检查伏笔是否已回收"""
        # 实际项目中应该根据伏笔类型和事件内容判断
        return False
    
    def get_constraint_injection(self) -> str:
        """生成约束注入文本（确保生成符合当前世界状态）"""
        constraints = []
        # 强制当前人物位置
        for entity_id, data in self.state['entities'].items():
            if data.get('location'):
                constraints.append(f"- {entity_id}当前位于{data['location']}")
        # 强制未解决伏笔
        for fs in self.state['active_foreshadowings'][:3]:  # 只注入最关键的3个
            constraints.append(f"- 必须推进伏笔: {fs['description']}")
        return "\n".join(constraints)

class GenerationError(Exception):
    pass

class NarrativeGenerationEngine:
    def __init__(self, config_path: str, project_id: str, narrative_tensor: Dict):
        self.config = self._load_config(config_path)
        self.project_id = project_id
        self.tensor = narrative_tensor
        self.state_db = NarrativeDatabase(f"projects/{project_id}/state.db")
        self.api_router = UniversalAPIRouter()
        
        # 初始化量子态世界
        self.world_state = QuantumWorldState(
            initial_entities=narrative_tensor['entity_space'],
            foreclosure_list=narrative_tensor['foreshadowing_field']
        )
    
    def _load_config(self, config_path: str) -> Dict:
        """加载配置文件"""
        # 实际项目中应该从文件加载
        return {
            "generation_mode": "continuation",
            "target_genre": "same",
            "learning_rounds": [
                {
                    "id": "L1_PlotTopology",
                    "name": "情节拓扑构建",
                    "provider": "kimi",
                    "model": "kimi-k1.5-long-context",
                    "temperature": 0.3,
                    "top_p": 0.9,
                    "frequency_penalty": 0.1,
                    "presence_penalty": 0.1,
                    "max_tokens": 8192,
                    "context_window": 128000,
                    "input_data": ["narrative_tensor.foreshadowing_field", "narrative_tensor.world_rules"],
                    "constraint_checks": ["no_temporal_paradox", "resource_conservation", "character_arc_continuity"],
                    "output_format": "plot_graph_json"
                },
                {
                    "id": "L2_WorldStateInit",
                    "name": "世界状态初始化",
                    "provider": "local",
                    "type": "computation",
                    "algorithm": "quantum_world_state",
                    "initial_state": "narrative_tensor.entity_space",
                    "foreclosure_list": "narrative_tensor.foreshadowing_field",
                    "output": "world_state_quantum"
                },
                {
                    "id": "L3_SceneDrafting",
                    "name": "场景分镜起草",
                    "provider": "h200",
                    "model": "deepseek-v3",
                    "temperature": 0.6,
                    "context_window": 1000000,
                    "parallel_scenes": 3,
                    "constraint_injection": True,
                    "output_format": "scene_cards"
                },
                {
                    "id": "L4_DialogueSculpting",
                    "name": "对话声纹雕刻",
                    "provider": "lmstudio",
                    "model": "qwen2.5-32b",
                    "temperature": 0.8,
                    "character_voice_lock": True,
                    "rag_retrieval": {
                        "enabled": True,
                        "source": "canon_dialogues",
                        "top_k": 5,
                        "similarity_threshold": 0.85
                    },
                    "psychological_coherence": True
                },
                {
                    "id": "L5_ProsePolishing",
                    "name": "文风抛光",
                    "provider": "kimi",
                    "model": "kimi-k1.5",
                    "temperature": 0.4,
                    "style_transfer": {
                        "source": "narrative_tensor.embedding_vector",
                        "intensity": 0.7
                    },
                    "literary_devices": ["metaphor", "symbolism", "irony", "foreshadowing"]
                }
            ],
            "validation_rounds": [
                {
                    "id": "V1_OOC_Check",
                    "name": "人物OOC检测",
                    "provider": "volcano",
                    "model": "doubao-pro-vision",
                    "temperature": 0.1,
                    "check_type": "character_consistency",
                    "reference_profiles": "narrative_tensor.entity_space",
                    "threshold": 0.92,
                    "blocking": True
                },
                {
                    "id": "V2_CanonCompliance",
                    "name": "原作合规性检查",
                    "provider": "local",
                    "check_type": "foreshadowing_resolution",
                    "ruleset": "rules/foreclosure_checker.py",
                    "must_resolve": "all_critical_foreshadowings"
                },
                {
                    "id": "V3_NarrativeCoherence",
                    "name": "叙事连贯性",
                    "provider": "multi",
                    "models": ["kimi-k1.5", "deepseek-v3"],
                    "check_type": "logical_consistency",
                    "attack_types": ["plot_hole", "character_motivation_gap", "world_rule_violation"]
                }
            ],
            "output_combiner": {
                "strategy": "weighted_voting",
                "weights": {"L3_SceneDrafting": 0.3, "L4_DialogueSculpting": 0.3, "L5_ProsePolishing": 0.4}
            }
        }
    
    def _load_template(self, template_path: str) -> str:
        """加载提示模板"""
        # 实际项目中应该从文件加载
        return "请基于以下叙事张量生成内容：\n\n{{narrative_tensor}}"
    
    def _parse_generation_output(self, response: str, output_format: str) -> Dict:
        """解析生成输出"""
        # 实际项目中应该根据output_format解析
        return {"content": response}
    
    def _execute_computation_round(self, cfg: Dict) -> Dict:
        """执行本地计算轮次"""
        # 实际项目中应该根据algorithm执行计算
        return {"result": "computed"}
    
    async def _retrieve_similar_canon_scenes(self, query: str, top_k: int, threshold: float) -> str:
        """检索相似的原文场景"""
        # 实际项目中应该从原文检索相似场景
        return "相似场景：..."
    
    def _extract_character_actions(self, content: Dict) -> Dict:
        """提取人物行动"""
        # 实际项目中应该从生成内容中提取人物行动
        return {}
    
    async def _check_character_consistency(self, char_id: str, actions: List, profile: Dict, cfg: Dict) -> float:
        """检查人物一致性"""
        # 实际项目中应该调用模型检查人物一致性
        return 0.95
    
    async def _red_team_validation(self, cfg: Dict, generated_content: Dict) -> Dict:
        """红队验证"""
        # 实际项目中应该调用多个模型进行对抗性验证
        return {"score": 0.95, "issues": []}
    
    def _embed_text(self, text: str) -> np.ndarray:
        """文本嵌入"""
        # 实际项目中应该使用嵌入模型
        return np.zeros(768)
    
    async def _regenerate_with_fix(self, cfg: Dict, violations: List) -> Dict:
        """自动修复并重生成"""
        # 实际项目中应该根据违规情况调整参数重生成
        return {"content": "修复后的内容"}
    
    def _compile_final_narrative(self, results: Dict) -> str:
        """编译最终叙事"""
        # 实际项目中应该根据权重合并结果
        return "最终生成的叙事内容"
    
    async def execute_generation_round(self, round_id: str, 
                                       previous_output: Optional[Dict] = None,
                                       constraints: Optional[Dict] = None,
                                       seed: Optional[int] = None) -> Dict:
        """执行单轮生成"""
        cfg = next(r for r in self.config['learning_rounds'] if r['id'] == round_id)
        
        # 构建增强提示
        base_prompt = self._load_template(f"prompts/generation/{round_id}.txt")
        
        # 注入叙事张量（作者风格向量）
        style_vector = self.tensor['embedding_vector']
        world_rules = json.dumps(self.tensor['world_rules'], ensure_ascii=False)
        
        # 注入世界状态约束（量子态坍缩）
        if cfg.get('constraint_injection', False):
            constraint_text = self.world_state.get_constraint_injection()
        else:
            constraint_text = ""
        
        # RAG检索增强（针对对话轮次）
        rag_context = ""
        if cfg.get('rag_retrieval', {}).get('enabled'):
            rag_config = cfg['rag_retrieval']
            query = previous_output.get('summary', '') if previous_output else ''
            rag_context = await self._retrieve_similar_canon_scenes(
                query=query,
                top_k=rag_config['top_k'],
                threshold=rag_config['similarity_threshold']
            )
        
        augmented_prompt = base_prompt.format(
            narrative_tensor=json.dumps(self.tensor, ensure_ascii=False),
            world_rules=world_rules,
            style_vector=style_vector,
            constraints=constraint_text,
            rag_context=rag_context,
            previous_output=json.dumps(previous_output) if previous_output else "{}",
            target_genre=self.config.get('target_genre', 'same')
        )
        
        # 特殊处理：本地计算轮次（如世界状态初始化）
        if cfg.get('type') == 'computation':
            return self._execute_computation_round(cfg)
        
        # API生成
        response = await self.api_router.call_single(
            provider=cfg['provider'],
            model=cfg['model'],
            messages=[{'role': 'user', 'content': augmented_prompt}],
            temperature=cfg['temperature'],
            top_p=cfg.get('top_p', 0.9),
            frequency_penalty=cfg.get('frequency_penalty', 0),
            presence_penalty=cfg.get('presence_penalty', 0),
            max_tokens=cfg['max_tokens']
        )
        
        # 解析结构化输出
        parsed = self._parse_generation_output(response, cfg['output_format'])
        
        # 更新世界状态（坍缩）
        self.world_state.update_from_generation(round_id, parsed)
        
        return {
            'round_id': round_id,
            'content': parsed,
            'world_state_hash': self.world_state.history_hash,
            'generation_params': {
                'temperature': cfg['temperature'],
                'model': cfg['model']
            }
        }
    
    async def execute_validation_round(self, round_id: str, 
                                       generated_content: Dict) -> Dict:
        """生成验证"""
        cfg = next(r for r in self.config['validation_rounds'] if r['id'] == round_id)
        
        if cfg['check_type'] == 'character_consistency':
            # 提取所有对话和行动，匿名化测试人物一致性
            dialogues = self._extract_character_actions(generated_content['content'])
            results = []
            for char_id, actions in dialogues.items():
                profile = self.tensor['entity_space'].get(char_id, {})
                score = await self._check_character_consistency(char_id, actions, profile, cfg)
                results.append({'character': char_id, 'score': score})
            
            min_score = min(r['score'] for r in results)
            return {
                'passed': min_score >= cfg['threshold'],
                'score': min_score,
                'violations': [r for r in results if r['score'] < cfg['threshold']]
            }
            
        elif cfg['check_type'] == 'foreshadowing_resolution':
            # 检查必须回收的伏笔是否已解决
            unresolved = self.world_state.state['active_foreshadowings']
            critical_unresolved = [f for f in unresolved if f.get('critical', False)]
            return {
                'passed': len(critical_unresolved) == 0,
                'score': 1.0 - (len(critical_unresolved) * 0.1),
                'unresolved': critical_unresolved
            }
            
        elif cfg['check_type'] == 'logical_consistency':
            # 多模型对抗性验证（红队）
            return await self._red_team_validation(cfg, generated_content)
    
    async def run_generation_pipeline(self, outline: Optional[Dict] = None) -> str:
        """运行完整生成流水线"""
        results = {}
        
        # 阶段1：元叙事到细纲（L1-L2，顺序）
        for cfg in self.config['learning_rounds'][:2]:
            data = await self.execute_generation_round(cfg['id'])
            results[cfg['id']] = data
            
            # 内联验证
            if cfg.get('inline_validation'):
                v_result = await self.execute_validation_round(cfg['inline_validation'], data)
                if not v_result['passed']:
                    # 自动修复：调整参数重新生成
                    data = await self._regenerate_with_fix(cfg, v_result['violations'])
        
        # 阶段2：多版本并行起草（L3，并行生成3个版本）
        draft_cfg = next(c for c in self.config['learning_rounds'] if c['id'] == 'L3_SceneDrafting')
        draft_tasks = [self.execute_generation_round('L3_SceneDrafting', seed=i) 
                      for i in range(draft_cfg.get('parallel_scenes', 1))]
        draft_results = await asyncio.gather(*draft_tasks)
        
        # 选择最佳版本（基于与原作风格的相似度）
        best_draft = self._select_best_draft(draft_results, self.tensor['embedding_vector'])
        results['L3_SceneDrafting'] = best_draft
        
        # 阶段3：细化与融合（L4-L5，顺序）
        for cfg in self.config['learning_rounds'][3:]:
            data = await self.execute_generation_round(cfg['id'], results.get('L3_SceneDrafting'))
            results[cfg['id']] = data
        
        # 最终验证（V1-V3）
        for cfg in self.config['validation_rounds']:
            v_result = await self.execute_validation_round(cfg['id'], results)
            if not v_result['passed'] and cfg.get('blocking', True):
                # 非阻塞失败记录警告，阻塞失败抛出异常
                raise GenerationError(f"Validation {cfg['id']} failed: {v_result}")
        
        # 编译最终文本
        final_text = self._compile_final_narrative(results)
        return final_text
    
    def _select_best_draft(self, drafts: List[Dict], target_vector: np.ndarray) -> Dict:
        """基于向量相似度选择最佳草稿"""
        if not drafts:
            return {"content": "默认草稿内容"}
        
        best_score = -1
        best_draft = drafts[0]
        for draft in drafts:
            # 计算草稿的嵌入向量与原作向量的余弦相似度
            draft_vector = self._embed_text(json.dumps(draft['content']))
            similarity = np.dot(draft_vector, target_vector) / (np.linalg.norm(draft_vector) * np.linalg.norm(target_vector))
            if similarity > best_score:
                best_score = similarity
                best_draft = draft
        return best_draft
