import asyncio
import sys
from core.analyzer import CanonAnalysisEngine
from core.generator import NarrativeGenerationEngine
from ui.main_window import main as gui_main

async def example_usage():
    """示例用法"""
    # 1. 分析阶段
    analyzer = CanonAnalysisEngine("config/analyzer_schema.json", "test_project")
    
    # 示例文本
    sample_text = ""
    try:
        with open("sample_canon.txt", "r", encoding="utf-8") as f:
            sample_text = f.read()
    except FileNotFoundError:
        print("示例文本文件不存在，使用默认文本")
        sample_text = "第一章 觉醒\n\n在一个风雨交加的夜晚，艾莉亚从梦中惊醒..."
    
    # 运行分析流水线
    narrative_tensor = await analyzer.run_analysis_pipeline(sample_text)
    print("分析完成，叙事张量已生成")
    
    # 2. 生成阶段
    generator = NarrativeGenerationEngine("config/generator_schema.json", "test_project", narrative_tensor)
    
    # 运行生成流水线
    generated_text = await generator.run_generation_pipeline()
    print("生成完成，结果已输出")
    
    # 保存生成结果
    with open("generated_output.txt", "w", encoding="utf-8") as f:
        f.write(generated_text)

if __name__ == "__main__":
    # 检查命令行参数
    if len(sys.argv) > 1 and sys.argv[1] == "--gui":
        # 启动GUI模式
        gui_main()
    else:
        # 启动命令行模式
        asyncio.run(example_usage())
