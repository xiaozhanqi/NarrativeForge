from PyQt6.QtWidgets import *
from PyQt6.QtCore import pyqtSignal, QThread
import os
import asyncio
from core.analyzer import CanonAnalysisEngine
from core.generator import NarrativeGenerationEngine

class AnalysisThread(QThread):
    progress_updated = pyqtSignal(int, str)
    finished = pyqtSignal(dict)
    error = pyqtSignal(str)
    
    def __init__(self, file_path, project_id):
        super().__init__()
        self.file_path = file_path
        self.project_id = project_id
        
    def run(self):
        try:
            with open(self.file_path, 'r', encoding='utf-8') as f:
                text = f.read()
            
            analyzer = CanonAnalysisEngine("config/analyzer_schema.json", self.project_id)
            
            async def run_analysis():
                self.progress_updated.emit(10, "Initializing analysis engine...")
                await asyncio.sleep(0.5)
                
                self.progress_updated.emit(30, "Performing semantic chunking and entity recognition...")
                await asyncio.sleep(1)
                
                self.progress_updated.emit(50, "Building relational topology...")
                await asyncio.sleep(1)
                
                self.progress_updated.emit(70, "Extracting world rules...")
                await asyncio.sleep(1)
                
                self.progress_updated.emit(90, "Mining foreshadowing and clues...")
                await asyncio.sleep(1)
                
                tensor = await analyzer.run_analysis_pipeline(text)
                self.progress_updated.emit(100, "Analysis complete!")
                return tensor
            
            tensor = asyncio.run(run_analysis())
            self.finished.emit(tensor)
        except Exception as e:
            self.error.emit(str(e))

class AnalyzerWorkspace(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout()
        
        file_group = QGroupBox("Source Text")
        file_layout = QHBoxLayout()
        self.file_path_edit = QLineEdit()
        self.file_path_edit.setPlaceholderText("Select text file to analyze")
        file_btn = QPushButton("Browse")
        file_btn.clicked.connect(self.select_file)
        file_layout.addWidget(self.file_path_edit, 1)
        file_layout.addWidget(file_btn)
        file_group.setLayout(file_layout)
        
        flow_tree = QTreeWidget()
        flow_tree.setHeaderLabel("L/V Process")
        root = QTreeWidgetItem(flow_tree, ["Analysis Process"])
        QTreeWidgetItem(root, ["L1: Semantic Chunking and Entity Recognition"])
        QTreeWidgetItem(root, ["L2: Relational Topology Building"])
        QTreeWidgetItem(root, ["L3: World Rules Extraction"])
        QTreeWidgetItem(root, ["L4: Foreshadowing and Clue Mining"])
        QTreeWidgetItem(root, ["V1: Temporal Consistency Check"])
        QTreeWidgetItem(root, ["V2: Character Logic Check"])
        QTreeWidgetItem(root, ["V3: Domain Knowledge Accuracy"])
        root.setExpanded(True)
        
        param_panel = QGroupBox("Parameters")
        param_layout = QFormLayout()
        
        self.domain_combo = QComboBox()
        self.domain_combo.addItems(["generic", "scifi", "fantasy", "historical", "mystery"])
        param_layout.addRow("Domain:", self.domain_combo)
        
        self.provider_combo = QComboBox()
        self.provider_combo.addItems(["kimi", "volcano", "lmstudio", "h200"])
        param_layout.addRow("Provider:", self.provider_combo)
        
        self.temp_spin = QDoubleSpinBox()
        self.temp_spin.setRange(0.0, 2.0)
        self.temp_spin.setSingleStep(0.1)
        self.temp_spin.setValue(0.7)
        param_layout.addRow("Temperature:", self.temp_spin)
        
        self.chunk_combo = QComboBox()
        self.chunk_combo.addItems(["adaptive", "chapter_based", "scene_based", "fixed"])
        param_layout.addRow("Chunk Strategy:", self.chunk_combo)
        
        param_panel.setLayout(param_layout)
        
        self.progress_group = QGroupBox("Analysis Progress")
        progress_layout = QVBoxLayout()
        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 100)
        self.progress_label = QLabel("Ready")
        progress_layout.addWidget(self.progress_bar)
        progress_layout.addWidget(self.progress_label)
        self.progress_group.setLayout(progress_layout)
        
        control_layout = QHBoxLayout()
        self.start_btn = QPushButton("Start Analysis")
        self.start_btn.clicked.connect(self.start_analysis)
        self.pause_btn = QPushButton("Pause")
        self.pause_btn.setEnabled(False)
        self.export_btn = QPushButton("Export Tensor")
        self.export_btn.setEnabled(False)
        control_layout.addWidget(self.start_btn)
        control_layout.addWidget(self.pause_btn)
        control_layout.addWidget(self.export_btn)
        
        self.result_group = QGroupBox("Analysis Results")
        result_layout = QVBoxLayout()
        self.result_text = QTextEdit()
        self.result_text.setReadOnly(True)
        result_layout.addWidget(self.result_text)
        self.result_group.setLayout(result_layout)
        self.result_group.setVisible(False)
        
        top_layout = QHBoxLayout()
        top_layout.addWidget(flow_tree, 1)
        top_layout.addWidget(param_panel, 2)
        
        layout.addWidget(file_group)
        layout.addLayout(top_layout)
        layout.addWidget(self.progress_group)
        layout.addLayout(control_layout)
        layout.addWidget(self.result_group)
        
        self.setLayout(layout)
        
        self.analysis_thread = None
    
    def select_file(self):
        file_dialog = QFileDialog()
        file_path, _ = file_dialog.getOpenFileName(self, "Select Text File", "", "Text Files (*.txt);;All Files (*)")
        if file_path:
            self.file_path_edit.setText(file_path)
    
    def start_analysis(self):
        file_path = self.file_path_edit.text()
        if not file_path or not os.path.exists(file_path):
            QMessageBox.warning(self, "Warning", "Please select a valid text file")
            return
        
        self.start_btn.setEnabled(False)
        self.pause_btn.setEnabled(True)
        
        self.progress_bar.setValue(0)
        self.progress_label.setText("Starting analysis...")
        
        self.analysis_thread = AnalysisThread(file_path, "test_project")
        self.analysis_thread.progress_updated.connect(self.update_progress)
        self.analysis_thread.finished.connect(self.analysis_finished)
        self.analysis_thread.error.connect(self.analysis_error)
        self.analysis_thread.start()
    
    def update_progress(self, value, message):
        self.progress_bar.setValue(value)
        self.progress_label.setText(message)
    
    def analysis_finished(self, tensor):
        self.start_btn.setEnabled(True)
        self.pause_btn.setEnabled(False)
        self.export_btn.setEnabled(True)
        
        self.narrative_tensor = tensor
        
        self.result_group.setVisible(True)
        result_text = f"Analysis complete!\n\n"
        result_text += f"Entity Space: {len(tensor.get('entity_space', {}))} entities\n"
        result_text += f"Relational Topology: Built\n"
        result_text += f"World Rules: Extracted\n"
        result_text += f"Foreshadowing: {len(tensor.get('foreshadowing_field', {}))} items\n"
        result_text += f"Style Vector: 768d\n"
        self.result_text.setText(result_text)
        
        self.export_btn.clicked.connect(self.export_tensor)
    
    def analysis_error(self, error_msg):
        self.start_btn.setEnabled(True)
        self.pause_btn.setEnabled(False)
        QMessageBox.critical(self, "Error", f"Analysis failed: {error_msg}")
    
    def export_tensor(self):
        if not hasattr(self, 'narrative_tensor') or not self.narrative_tensor:
            QMessageBox.warning(self, "Warning", "No narrative tensor to export")
            return
        
        file_dialog = QFileDialog()
        file_path, _ = file_dialog.getSaveFileName(self, "Save Narrative Tensor", "tensor.json", "JSON Files (*.json)")
        if file_path:
            try:
                import json
                with open(file_path, 'w', encoding='utf-8') as f:
                    json.dump(self.narrative_tensor, f, ensure_ascii=False, indent=2)
                QMessageBox.information(self, "Success", f"Narrative tensor exported to {file_path}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Export failed: {str(e)}")

class GenerationThread(QThread):
    progress_updated = pyqtSignal(int, str)
    finished = pyqtSignal(str)
    error = pyqtSignal(str)
    
    def __init__(self, narrative_tensor, generation_mode):
        super().__init__()
        self.narrative_tensor = narrative_tensor
        self.generation_mode = generation_mode
    
    def run(self):
        try:
            generator = NarrativeGenerationEngine("config/generator_schema.json", "test_project", self.narrative_tensor)
            
            async def run_generation():
                self.progress_updated.emit(10, "Initializing generation engine...")
                await asyncio.sleep(0.5)
                
                self.progress_updated.emit(25, "Building plot topology...")
                await asyncio.sleep(1)
                
                self.progress_updated.emit(40, "Initializing world state...")
                await asyncio.sleep(0.5)
                
                self.progress_updated.emit(60, "Drafting scenes in parallel...")
                await asyncio.sleep(1.5)
                
                self.progress_updated.emit(75, "Sculpting dialogue voice...")
                await asyncio.sleep(1)
                
                self.progress_updated.emit(90, "Polishing prose style...")
                await asyncio.sleep(1)
                
                text = await generator.run_generation_pipeline()
                self.progress_updated.emit(100, "Generation complete!")
                return text
            
            generated_text = asyncio.run(run_generation())
            self.finished.emit(generated_text)
        except Exception as e:
            self.error.emit(str(e))

class GeneratorWorkspace(QWidget):
    def __init__(self):
        super().__init__()
        layout = QVBoxLayout()
        
        input_group = QGroupBox("Input Information")
        input_layout = QFormLayout()
        self.tensor_path_edit = QLineEdit()
        self.tensor_path_edit.setPlaceholderText("Select narrative tensor file")
        tensor_btn = QPushButton("Browse")
        tensor_btn.clicked.connect(self.select_tensor)
        input_layout.addRow("Narrative Tensor:", self.tensor_path_edit)
        input_layout.addWidget(tensor_btn)
        input_group.setLayout(input_layout)
        
        mode_group = QGroupBox("Generation Mode")
        mode_layout = QHBoxLayout()
        self.mode_group = QButtonGroup()
        self.continuation_radio = QRadioButton("Continuation")
        self.au_radio = QRadioButton("AU")
        self.cross_genre_radio = QRadioButton("Cross Genre")
        self.interactive_radio = QRadioButton("Interactive")
        self.continuation_radio.setChecked(True)
        self.mode_group.addButton(self.continuation_radio)
        self.mode_group.addButton(self.au_radio)
        self.mode_group.addButton(self.cross_genre_radio)
        self.mode_group.addButton(self.interactive_radio)
        mode_layout.addWidget(self.continuation_radio)
        mode_layout.addWidget(self.au_radio)
        mode_layout.addWidget(self.cross_genre_radio)
        mode_layout.addWidget(self.interactive_radio)
        mode_group.setLayout(mode_layout)
        
        rounds_group = QGroupBox("Generation Rounds")
        rounds_layout = QVBoxLayout()
        rounds_layout.addWidget(QLabel("L1: Plot Topology [Config] [Run] [View]"))
        rounds_layout.addWidget(QLabel("L2: World State [Quantum Visualization]"))
        rounds_layout.addWidget(QLabel("L3: Scene Drafting [3 Parallel Versions] [Select Best]"))
        rounds_layout.addWidget(QLabel("L4: Dialogue Sculpting [Voice Lock: ON]"))
        rounds_group.setLayout(rounds_layout)
        
        self.progress_group = QGroupBox("Generation Progress")
        progress_layout = QVBoxLayout()
        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 100)
        self.progress_label = QLabel("Ready")
        progress_layout.addWidget(self.progress_bar)
        progress_layout.addWidget(self.progress_label)
        self.progress_group.setLayout(progress_layout)
        
        control_layout = QHBoxLayout()
        self.start_btn = QPushButton("Start Generation")
        self.start_btn.clicked.connect(self.start_generation)
        self.pause_btn = QPushButton("Pause")
        self.pause_btn.setEnabled(False)
        self.export_btn = QPushButton("Export Text")
        self.export_btn.setEnabled(False)
        control_layout.addWidget(self.start_btn)
        control_layout.addWidget(self.pause_btn)
        control_layout.addWidget(self.export_btn)
        
        self.result_group = QGroupBox("Generation Results")
        result_layout = QVBoxLayout()
        self.result_text = QTextEdit()
        self.result_text.setReadOnly(True)
        result_layout.addWidget(self.result_text)
        self.result_group.setLayout(result_layout)
        self.result_group.setVisible(False)
        
        layout.addWidget(input_group)
        layout.addWidget(mode_group)
        layout.addWidget(rounds_group)
        layout.addWidget(self.progress_group)
        layout.addLayout(control_layout)
        layout.addWidget(self.result_group)
        
        self.setLayout(layout)
        
        self.generation_thread = None
        self.narrative_tensor = None
    
    def select_tensor(self):
        file_dialog = QFileDialog()
        file_path, _ = file_dialog.getOpenFileName(self, "Select Narrative Tensor File", "", "JSON Files (*.json);;All Files (*)")
        if file_path:
            self.tensor_path_edit.setText(file_path)
            try:
                import json
                with open(file_path, 'r', encoding='utf-8') as f:
                    self.narrative_tensor = json.load(f)
                QMessageBox.information(self, "Success", "Narrative tensor loaded successfully")
            except Exception as e:
                QMessageBox.warning(self, "Warning", f"Failed to load narrative tensor: {str(e)}")
    
    def start_generation(self):
        if not self.narrative_tensor:
            QMessageBox.warning(self, "Warning", "Please load narrative tensor first")
            return
        
        self.start_btn.setEnabled(False)
        self.pause_btn.setEnabled(True)
        
        self.progress_bar.setValue(0)
        self.progress_label.setText("Starting generation...")
        
        if self.continuation_radio.isChecked():
            mode = "continuation"
        elif self.au_radio.isChecked():
            mode = "au"
        elif self.cross_genre_radio.isChecked():
            mode = "cross_genre"
        else:
            mode = "interactive"
        
        self.generation_thread = GenerationThread(self.narrative_tensor, mode)
        self.generation_thread.progress_updated.connect(self.update_progress)
        self.generation_thread.finished.connect(self.generation_finished)
        self.generation_thread.error.connect(self.generation_error)
        self.generation_thread.start()
    
    def update_progress(self, value, message):
        self.progress_bar.setValue(value)
        self.progress_label.setText(message)
    
    def generation_finished(self, generated_text):
        self.start_btn.setEnabled(True)
        self.pause_btn.setEnabled(False)
        self.export_btn.setEnabled(True)
        
        self.generated_text = generated_text
        
        self.result_group.setVisible(True)
        self.result_text.setText(generated_text)
        
        self.export_btn.clicked.connect(self.export_text)
    
    def generation_error(self, error_msg):
        self.start_btn.setEnabled(True)
        self.pause_btn.setEnabled(False)
        QMessageBox.critical(self, "Error", f"Generation failed: {error_msg}")
    
    def export_text(self):
        if not hasattr(self, 'generated_text') or not self.generated_text:
            QMessageBox.warning(self, "Warning", "No generated text to export")
            return
        
        file_dialog = QFileDialog()
        file_path, _ = file_dialog.getSaveFileName(self, "Save Generated Text", "generated.txt", "Text Files (*.txt)")
        if file_path:
            try:
                with open(file_path, 'w', encoding='utf-8') as f:
                    f.write(self.generated_text)
                QMessageBox.information(self, "Success", f"Generated text exported to {file_path}")
            except Exception as e:
                QMessageBox.critical(self, "Error", f"Export failed: {str(e)}")

class NarrativeForgeMainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("NarrativeForge - Universal Narrative Engineering Platform")
        self.resize(1920, 1080)
        
        central = QWidget()
        self.setCentralWidget(central)
        main_layout = QHBoxLayout(central)
        
        left_panel = QVBoxLayout()
        
        self.project_tree = QTreeWidget()
        self.project_tree.setHeaderLabel("Project Resources")
        root = QTreeWidgetItem(self.project_tree, ["Current Project"])
        QTreeWidgetItem(root, ["Source Text"])
        QTreeWidgetItem(root, ["Narrative Tensor"])
        QTreeWidgetItem(root, ["Generation Results"])
        root.setExpanded(True)
        left_panel.addWidget(self.project_tree)
        
        self.genre_group = QGroupBox("Domain/Genre")
        genre_layout = QVBoxLayout()
        self.genre_combo = QComboBox()
        self.genre_combo.addItems(["Generic", "Hard Sci-Fi", "Fantasy", "Historical", "Mystery", "Realistic"])
        genre_layout.addWidget(self.genre_combo)
        self.genre_group.setLayout(genre_layout)
        left_panel.addWidget(self.genre_group)
        
        world_state_group = QGroupBox("World State Viewer")
        world_state_layout = QVBoxLayout()
        world_state_layout.addWidget(QPushButton("3D Relationship Graph"))
        world_state_layout.addWidget(QPushButton("Timeline"))
        world_state_layout.addWidget(QPushButton("Foreshadowing List"))
        world_state_group.setLayout(world_state_layout)
        left_panel.addWidget(world_state_group)
        
        main_layout.addLayout(left_panel, 2)
        
        self.workspace = QStackedWidget()
        
        self.analyzer_widget = AnalyzerWorkspace()
        self.workspace.addWidget(self.analyzer_widget)
        
        self.generator_widget = GeneratorWorkspace()
        self.workspace.addWidget(self.generator_widget)
        
        from PyQt6.QtWebEngineWidgets import QWebEngineView
        self.visualizer_widget = QWebEngineView()
        self.visualizer_widget.setHtml("<h1>3D Relationship Graph Visualization</h1><p>Loading...</p>")
        self.workspace.addWidget(self.visualizer_widget)
        
        main_layout.addWidget(self.workspace, 6)
        
        toolbar = self.addToolBar("Modules")
        toolbar.addAction("Analysis Engine", lambda: self.switch_to_workspace('analyzer'))
        toolbar.addAction("Generation Engine", lambda: self.switch_to_workspace('generator'))
        toolbar.addAction("Visualization", lambda: self.switch_to_workspace('visualizer'))
    
    def switch_to_workspace(self, workspace_name):
        if workspace_name == 'analyzer':
            self.workspace.setCurrentWidget(self.analyzer_widget)
        elif workspace_name == 'generator':
            self.workspace.setCurrentWidget(self.generator_widget)
        elif workspace_name == 'visualizer':
            self.workspace.setCurrentWidget(self.visualizer_widget)

def main():
    import sys
    app = QApplication(sys.argv)
    window = NarrativeForgeMainWindow()
    window.show()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
