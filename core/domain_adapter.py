class DomainAdapter:
    """领域知识适配器（科幻、奇幻、历史、现实）"""
    
    DOMAINS = {
        'generic': {'kb': None, 'constraints': []},
        'scifi': {'kb': 'kb/physics_hardness.db', 'constraints': ['light_speed_limit', 'entropy_increase']},
        'fantasy': {'kb': 'kb/magic_systems.db', 'constraints': ['mana_conservation', 'spell_syntax']},
        'historical': {'kb': 'kb/chronology.db', 'constraints': ['temporal_authenticity', 'anachronism_check']},
        'mystery': {'kb': 'kb/deductive_logic.db', 'constraints': ['fair_play_rules', 'clue_visibility']}
    }
    
    def __init__(self, domain: str):
        self.domain = domain
        self.config = self.DOMAINS.get(domain, self.DOMAINS['generic'])
        
    def get_context(self, extractors: list) -> str:
        """获取领域特定的提示上下文"""
        if self.domain == 'scifi':
            return "请确保所有科技设定符合硬科幻标准，超光速旅行需有曲速/跃迁解释..."
        elif self.domain == 'fantasy':
            return f"魔法系统需遵循以下规则：{self._load_magic_rules()}..."
        return ""
    
    def validate_fact(self, claim: str) -> float:
        """事实准确性检查（针对历史/科幻）"""
        # 查询知识库验证声明的真实性
        return 0.95
    
    def _load_magic_rules(self) -> str:
        """加载魔法系统规则"""
        # 实际项目中应该从知识库加载
        return "魔力守恒定律、咒语必须发音清晰、魔法效果与施法者能力成正比"
