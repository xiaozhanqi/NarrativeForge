import asyncio
import json
from typing import List, Dict, Any

class AsyncRateLimiter:
    def __init__(self, max_calls: int):
        self.max_calls = max_calls
        self.semaphore = asyncio.Semaphore(max_calls)

    async def __aenter__(self):
        await self.semaphore.acquire()
        return self

    async def __aexit__(self, exc_type, exc_val, exc_tb):
        self.semaphore.release()

class UniversalAPIRouter:
    def __init__(self):
        self.rate_limiters = {
            'kimi': AsyncRateLimiter(60),
            'volcano': AsyncRateLimiter(120),
            'lmstudio': AsyncRateLimiter(1000),  # 本地不限
            'h200': AsyncRateLimiter(100),  # 本地高性能集群
            'openai': AsyncRateLimiter(100)
        }
        self.health_status = {k: True for k in self.rate_limiters.keys()}
        
    def _build_payload(self, provider: str, model: str, messages: List[Dict], kwargs: Dict) -> Dict:
        """适配不同API格式"""
        base = {
            'model': model,
            'messages': messages,
            'temperature': kwargs.get('temperature', 0.7),
            'max_tokens': kwargs.get('max_tokens', 4096)
        }
        
        if provider == 'kimi':
            base['top_p'] = kwargs.get('top_p', 0.9)
            if 'response_format' in kwargs:
                base['response_format'] = kwargs['response_format']
        elif provider == 'volcano':
            # 火山引擎特定格式
            base = {
                'model': model,
                'messages': messages,
                'temperature': kwargs.get('temperature', 0.7),
                'max_tokens': kwargs.get('max_tokens', 4096)
            }
        elif provider == 'lmstudio':
            # OpenAI兼容格式
            pass
            
        return base
    
    async def call_single(self, provider: str, model: str, messages: List[Dict], **kwargs) -> str:
        """调用单个API"""
        # 这里是模拟实现，实际项目中需要根据不同provider调用相应的API
        await asyncio.sleep(1)  # 模拟API调用延迟
        return json.dumps({"content": "模拟API响应"})
    
    async def batch_call(self, tasks: List[Dict], max_concurrent: int = 4, progress_callback = None) -> List[Dict]:
        """批量调用API"""
        results = []
        semaphore = asyncio.Semaphore(max_concurrent)
        
        async def process_task(task, idx):
            async with semaphore:
                provider = task['provider']
                async with self.rate_limiters[provider]:
                    # 模拟API调用
                    await asyncio.sleep(0.5)
                    if progress_callback:
                        progress_callback(idx + 1, len(tasks))
                    return {"content": f"模拟响应 {idx+1}"}
        
        task_coros = [process_task(task, idx) for idx, task in enumerate(tasks)]
        results = await asyncio.gather(*task_coros)
        
        return results