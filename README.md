# NetDesigner

NetDesigner is a zero-dependency, from-scratch C++ neural network architecture and visual graph modeling suite. Built without third-party machine learning frameworks, it translates visual circuit-like schematics directly into high-performance, pointer-connected compute graphs for training and inference.

NetDesigner is designed for developers, roboticists, and systems engineers who require transparent, low-level execution control, cross-platform deployment, and custom topological flexibility.

## Architectural Highlights

* **Pure Native C++ Engine:** Zero dependency on external machine learning or tensor libraries (no PyTorch, TensorFlow, or BLAS runtimes required).
* **Pointer-Connected Graph:** Execution passes evaluate directly through raw memory pointer offsets (`int64-t`), eliminating dynamic dictionary lookups and array overhead during passes.
* **Heterogeneous Node Activations:** Dynamic per-node activation assignment (ReLU, Sigmoid, Tanh, Shockley diode physics, Softmax, Gaussian) with user-configurable leakage parameters to mitigate saturation and dead gradients.
* **Visual Topology Compilation:** Integrates a visual drawing environment with a topological resolution engine, automatically ordering forward and backward passes from user-drawn connection schematics.
* **Cross-Platform Portability:** Dual serialization support—native Windows MFC archive structures (`.nnd`) paired with platform-agnostic JSON (`.json`) for headless execution on embedded Linux devices, robotics platforms, and autonomous systems.



## System Architecture

The software is structured into three decoupled functional layers:

┌─────────────────────────────────────────────────────────────┐
│ Visual UI Layout Layer │
│ (MFC CDocument / CView Schematics, Component Canvas) │
└──────────────────────────────┬──────────────────────────────┘
│
▼
┌─────────────────────────────────────────────────────────────┐
│ Intermediate Data Representation │
│ (NetData / netDataStruct: Topology, Edges, Parameters) │
│ Serialization: Native .nnd / JSON │
└──────────────────────────────┬──────────────────────────────┘
│
▼
┌─────────────────────────────────────────────────────────────┐
│ Graph Resolution and Execution │
│ (NetBuilder: Depth-First Inlining, Topological Sort) │
│ (CNetCore: Forward / Reverse Backpropagation Passes) │
└─────────────────────────────────────────────────────────────┘



## Core Computational Primitives (`Neuron.h` / `Neuron.cpp`)

All structural primitives derive from or compose `OutputNode`, which centralizes weight allocations, gradient deltas, and pointer-bound memory paths.

|Component|Class Type|Description|
|-|-|-|
|**OutputNode**|Base Primitive|Atomic building block. Manages `inputpointers` and multi-path `backpropinputpointers`. Evaluates per-node activation functions and derivatives dynamically.|
|**HiddenNode**|Derived Primitive|Dense internal hidden node inheriting multi-path backprop capability with fine-grained parameter updates.|
|**HiddenNet**|Dense Backbone|Multi-layer, fully connected array parameterized by depth and width. Supports Windows multicore parallel thread dispatch for layers exceeding size thresholds.|
|**HiddenConvolution**|Spatial Operator|Multi-modal convolution engine. Features dedicated kernels for $N$-D generic sequences, fully-connected 3-channel RGB image mapping, and parameter-efficient shared-weight processing. Includes automatic spatial buffering for generative pipelines.|
|**Attention / SelfAttention**|Sequence Mapping|Zero-dependency Scaled Dot-Product Attention ($Q, K, V$). Includes configurable Softmax and an integrated **Sum Add Limit** (epsilon clamping) to prevent division-by-zero errors and exploding gradients during backpropagation. `SelfAttention` auto-infers optimal dimensions directly from input size.|
|**Addition / Multiplication**|Operator Nodes|Element-wise residual and skip operators. Automatically handles dimension mismatches through graceful index alignment and fall-off.|
|**Dropout**|Regularization|Dynamic, stream-level stochastic zeroing of image pixels or sequence elements with zero intermediate mask allocation overhead.|
|**NetCluster**|Hierarchical Assembly|Container for nesting sub-graphs. Enables visual modularity through cloning and parameterized variants.|



## Graph Resolution \& Topological Sorting

To turn arbitrary 2D drawn connections into a deterministic compute graph, `NetBuilder::initialize` processes the visual state through a dependency resolution pipeline:

1. **Recursive Depth-First Assembly Unpacking:** All nested `NetCluster` assemblies are flattened deepest-first, inlining internal nodes and edge references directly into the primary working instance of `NetData`.
2. **Dependency Resolution (Topological Sort):** Uses connection tracking (`findSupportingNodes` and `findFollowingNodes`) to iteratively schedule nodes whose inputs are satisfied, resolving arbitrary schematic connections into a valid Directed Acyclic Graph (DAG).
3. **Symmetric Execution Pass Scheduling:**

   * **Forward Propagation:** Evaluated sequentially from index `0` to `N-1`.
* **Backpropagation:** Evaluated in reverse sequence from `N-1` down to `0`.
4. **Pointer Binding (`join()`):** Dynamically wires memory addresses between adjacent nodes and blocks, ensuring forward calculations and gradient distributions run directly across raw pointer offsets.



## File Formats \& Headless Deployment

* **`.nnd` Files:** Native binary archive format optimized for rapid state restoration within the Visual Studio / Windows UI environment.
* **`.json` Files:** Standardized structural schema containing complete graph topology, hyperparameters, weights, and connection indices. Designed to be parsed by headless C++ execution runtimes on Linux, making trained models portable to autonomous ground vehicles, UAVs, and edge processors.



## Building the Project

### Prerequisites

* Windows 10 / 11
* Visual Studio 2019 or Visual Studio 2022 (with **Desktop development with C++** and **MFC (Microsoft Foundation Classes)** workload installed)

### Compilation

1. Clone the repository: git clone \\\[https://github.com/](https://github.com/)\\\[Your-Username]/NetDesigner.git
2. Open NetDesigner.sln in Visual Studio.
3. Set your target configuration to Release and x64.
4. Build the solution (Ctrl + Shift + B).

