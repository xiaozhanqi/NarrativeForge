from flask import Flask, render_template, request, jsonify, send_file
from flask_cors import CORS
import os
import json
import asyncio
from core.analyzer import CanonAnalysisEngine
from core.generator import NarrativeGenerationEngine

app = Flask(__name__)
CORS(app)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/analyze', methods=['POST'])
def analyze():
    try:
        data = request.json
        text = data.get('text')
        project_id = data.get('project_id', 'web_project')
        
        if not text:
            return jsonify({'error': 'Text is required'}), 400
        
        analyzer = CanonAnalysisEngine("config/analyzer_schema.json", project_id)
        
        async def run_analysis():
            tensor = await analyzer.run_analysis_pipeline(text)
            return tensor
        
        tensor = asyncio.run(run_analysis())
        return jsonify(tensor)
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/generate', methods=['POST'])
def generate():
    try:
        data = request.json
        narrative_tensor = data.get('narrative_tensor')
        mode = data.get('mode', 'continuation')
        project_id = data.get('project_id', 'web_project')
        
        if not narrative_tensor:
            return jsonify({'error': 'Narrative tensor is required'}), 400
        
        generator = NarrativeGenerationEngine("config/generator_schema.json", project_id, narrative_tensor)
        
        async def run_generation():
            text = await generator.run_generation_pipeline()
            return text
        
        generated_text = asyncio.run(run_generation())
        return jsonify({'generated_text': generated_text})
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/health')
def health():
    return jsonify({'status': 'ok'})

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
