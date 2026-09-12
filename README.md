# SAPUDOM Structural Analysis Desktop V0.1

Native desktop foundation for macOS/Windows using C++17 + Qt 6.

## V0.1 features
- Native desktop window (no browser / localhost)
- New / Open / Save / Save As
- `.sapudom` project file (JSON-based in V0.1)
- 2D structural modeling canvas
- Grid snapping
- Create/select Nodes
- Draw Beam members
- Draw Column members
- Simple member chaining
- Basic project persistence

## Build on macOS

### 1. Install Apple command-line tools
```bash
xcode-select --install
```

### 2. Install Homebrew (if not already installed)
Use the official Homebrew installation instructions from https://brew.sh/

### 3. Install Qt and CMake
```bash
brew install qt cmake
```

### 4. Configure
From the project folder:
```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

### 5. Build
```bash
cmake --build build --config Release
```

### 6. Run
Apple Silicon / Homebrew builds usually produce the app under `build/SAPUDOM.app` or an executable named `SAPUDOM` in the build folder.

You can also open the generated project in Qt Creator if you prefer a GUI IDE.

## V0.2 recommended scope
- Structural database: Material + Section
- Support / restraints
- Nodal load + member UDL
- 2D frame element stiffness matrix
- Global matrix assembly
- Boundary-condition solver
- Displacement + reaction + member-end forces
- Result diagrams: N, V, M
