import subprocess
import sys
import os

def check_and_install_dependencies():
    """Check and install missing dependencies"""
    print("Checking Python environment...")
    print(f"Python version: {sys.version}")
    
    # Required packages
    required_packages = {
        'numpy': 'numpy',
        'networkx': 'networkx',
        'scikit-learn': 'scikit-learn',
        'aiohttp': 'aiohttp',
        'PyQt6': 'PyQt6',
        'PyQt6-WebEngine': 'PyQt6-WebEngine',
        'Flask': 'flask',
        'Flask-CORS': 'flask-cors',
        'jsonschema': 'jsonschema',
        'python-dotenv': 'python-dotenv'
    }
    
    missing_packages = []
    
    # Check each package
    for package_name, import_name in required_packages.items():
        try:
            __import__(import_name)
            print(f"✓ {package_name} is installed")
        except ImportError:
            print(f"✗ {package_name} is missing")
            missing_packages.append(package_name)
    
    # Install missing packages
    if missing_packages:
        print(f"\nInstalling missing packages: {', '.join(missing_packages)}")
        for package in missing_packages:
            try:
                subprocess.check_call([sys.executable, '-m', 'pip', 'install', package])
                print(f"✓ {package} installed successfully")
            except subprocess.CalledProcessError as e:
                print(f"✗ Failed to install {package}: {e}")
                return False
    else:
        print("\nAll required packages are installed!")
    
    return True

def check_directories():
    """Check and create required directories"""
    print("\nChecking directories...")
    directories = ['projects', 'projects/test_project']
    
    for directory in directories:
        if not os.path.exists(directory):
            os.makedirs(directory, exist_ok=True)
            print(f"✓ Created directory: {directory}")
        else:
            print(f"✓ Directory exists: {directory}")

def run_application(mode='gui'):
    """Run the NarrativeForge application"""
    print(f"\nStarting NarrativeForge in {mode} mode...")
    
    if mode == 'gui':
        try:
            from ui.main_window import main as gui_main
            gui_main()
        except ImportError as e:
            print(f"Error importing GUI module: {e}")
            print("Please check that all dependencies are installed.")
            sys.exit(1)
    elif mode == 'web':
        try:
            from app import app
            print("Starting Flask web server...")
            print("Open your browser and navigate to: http://localhost:5000")
            app.run(debug=True, host='0.0.0.0', port=5000)
        except ImportError as e:
            print(f"Error importing web module: {e}")
            print("Please check that all dependencies are installed.")
            sys.exit(1)
    elif mode == 'cli':
        try:
            import asyncio
            from core.analyzer import CanonAnalysisEngine
            from core.generator import NarrativeGenerationEngine
            
            async def example_usage():
                analyzer = CanonAnalysisEngine("config/analyzer_schema.json", "test_project")
                
                sample_text = ""
                try:
                    with open("sample_canon.txt", "r", encoding="utf-8") as f:
                        sample_text = f.read()
                except FileNotFoundError:
                    print("Sample text file not found, using default text")
                    sample_text = "Chapter 1: Awakening\n\nOn a stormy night, Aria woke up from her dream..."
                
                print("Running analysis pipeline...")
                narrative_tensor = await analyzer.run_analysis_pipeline(sample_text)
                print("Analysis complete, narrative tensor generated")
                
                print("Running generation pipeline...")
                generator = NarrativeGenerationEngine("config/generator_schema.json", "test_project", narrative_tensor)
                generated_text = await generator.run_generation_pipeline()
                print("Generation complete, results output")
                
                with open("generated_output.txt", "w", encoding="utf-8") as f:
                    f.write(generated_text)
                print("Generated text saved to generated_output.txt")
            
            asyncio.run(example_usage())
        except ImportError as e:
            print(f"Error importing CLI modules: {e}")
            print("Please check that all dependencies are installed.")
            sys.exit(1)

def main():
    """Main entry point"""
    print("=" * 60)
    print("NarrativeForge - Environment Setup and Launcher")
    print("=" * 60)
    
    # Check and install dependencies
    if not check_and_install_dependencies():
        print("\nFailed to install required dependencies. Please check the error messages above.")
        sys.exit(1)
    
    # Check and create directories
    check_directories()
    
    # Determine run mode
    mode = 'gui'
    if len(sys.argv) > 1:
        mode = sys.argv[1]
    
    # Run application
    run_application(mode)

if __name__ == "__main__":
    main()
