# Feedforward Neural Network Parallelization

Parallel Programming laboratory project, Universitat Autònoma de Barcelona,
academic year 2026–2027. The goal is to parallelize the supplied C multilayer
perceptron and compare correctness, execution time, speedup, and efficiency
against its sequential implementation.

The [assignment](resources/CAP_Pra_ctica_NN_26_27.pdf) is the reference for scope
and deliverables. This repository currently contains the starter implementation
and a shared project scaffold; the parallel versions are still to be developed.

## Task brief

The network learns to recognize handwritten digits `0–9` from binary `32 × 32`
images. The supplied default configuration uses:

| Parameter | Default |
| --- | --- |
| Network layers | `1024 → 500 → 210 → 135 → 10` |
| Training / test patterns | 1,934 / 946, from OPTDIGITS |
| Epochs / batch size | 10 / 50 |
| Learning rate / configured seed | 0.015 / 50 |
| Activations | ReLU in hidden layers; sigmoid in the output layer |

Training is the main computational workload. Each epoch shuffles the patterns,
performs forward propagation and backpropagation, accumulates gradients within
a batch, and updates weights. Testing performs forward propagation and counts
correct predictions. MNIST-format training and test files are also supplied;
using them requires a matching configuration.

The development tracks are:

- **OpenMP:** shared-memory CPU parallelism; identify independent loops or tasks
  and handle shared activations, gradients, reductions, and synchronization.
- **OpenMPI:** distributed-memory parallelism using MPI; choose a decomposition
  that balances work while limiting communication and synchronization.
- **CUDA:** GPU implementation; identify suitable kernels and control data
  transfers and device memory use.

**Assignment clarification:** Section III.C specifies OpenACC for the accelerator
implementation and describes CUDA as an optional extension worth an extra
point. Other sections use “OpenACC-CUDA” or “CUDA.” The requested `cuda` branch
is provided, but the accepted accelerator deliverable should be confirmed with
the teaching staff before implementation.

For each delivery, provide functional source code and a PDF report describing
the parallelization strategy, code changes, build/run instructions, measured
speedup and efficiency, profiling results, and bottlenecks with possible
solutions. The assignment mentions delivery dates but does not give explicit
dates; consult the Virtual Campus schedule.

## Repository structure

```text
.
├── README.md                         # Project brief, layout, and workflow
├── .gitignore                        # Generated files and local metadata
├── resources/
│   └── CAP_Pra_ctica_NN_26_27.pdf     # Original assignment
├── code/
│   └── XXNN-Multinivell-main/         # Supplied sequential C project
│       ├── main.c / main.h           # Training/testing orchestration
│       ├── common/                   # Dataset loading and elapsed-time helper
│       ├── configuration/            # Configuration parser and default file
│       ├── initialize/               # Network allocation and initialization
│       ├── layer/                    # Layer data structure and memory handling
│       ├── randomizer/               # Random-number helpers
│       ├── training/                 # Forward/backpropagation and weight updates
│       ├── datasets/                 # OPTDIGITS and MNIST-format data
│       ├── seq.sub                   # Supplied sequential Slurm job script
│       ├── README.txt                # Original starter documentation
│       └── exec                      # Supplied Linux x86-64 executable
├── configs/                          # Future smoke-test and benchmark configs
├── scripts/                          # Future job, benchmark, and profiling scripts
├── results/                          # Future small measurement tables and summaries
└── reports/                          # Future report sources, figures, and final PDFs
```

The four new folders contain `.gitkeep` placeholders. Keep executable code in
the existing modular project and develop each implementation on its branch;
there is no need to duplicate the starter tree for each backend. Build products
belong in the ignored root `build/` directory. Large profiler traces and raw
logs belong in ignored `results/raw/` and `results/profiles/`; commit small
measurement tables and figures needed to reproduce the reports.

## Branch workflow

| Branch | Purpose |
| --- | --- |
| `main` | Shared scaffold and sequential reference |
| `openmp` | OpenMP implementation and analysis |
| `openmpi` | MPI implementation using OpenMPI and analysis |
| `cuda` | CUDA implementation and analysis |

All three implementation branches start from the shared scaffold commit.
Use `git switch openmp`, `git switch openmpi`, or `git switch cuda` to select
a track. Put common baseline fixes and documentation on `main`, then merge
them into each implementation branch. Record any changes to the sequential
reference so timing comparisons use equivalent workloads. Keep the original
starter recoverable through the repository's initial commit.

## Build and run the sequential code

Requirements: a C compiler and the C math library. From the repository root:

```sh
mkdir -p build
cd code/XXNN-Multinivell-main
cc -O2 -Wall -Wextra \
  main.c common/common.c configuration/config.c layer/layer.c \
  randomizer/randomizer.c initialize/initialize.c training/training.c \
  -o ../../build/nn-sequential -lm
../../build/nn-sequential
```

These are build/run instructions for the supplied code; see the baseline issues
below before relying on execution results. The supplied `exec` binary is for
Linux x86-64, so rebuild from source on your target machine.

The program defaults to `configuration/configfile.txt` and accepts an alternate
configuration as its first argument, for example:

```sh
# While in code/XXNN-Multinivell-main, after adding this configuration:
../../build/nn-sequential ../../configs/smoke.txt
```

Configuration fields must retain the order expected by `readConfiguration()`.
Dataset paths are resolved from the process's working directory, even when the
configuration is elsewhere. The default paths expect the starter directory
used above. Use `debug=0` for timing; `debug=1` prints intermediate state and
pauses interactively, while `debug=2` prints individual test predictions.

The supplied `seq.sub` builds with GCC and runs `perf stat` under Slurm. Review
its module and queue settings against the current cluster instructions before
submission. The assignment places OpenMP/MPI performance measurements on Wilma
and accelerator measurements on the architecture laboratory GPU queue.

## Next steps and baseline issues

Before implementing parallelism:

1. **Establish a correct sequential reference.** A local AddressSanitizer run
   confirms a heap-buffer overflow in `common.c:readImg()`: `%s` writes a
   terminating byte beyond the final row of the allocated image. Source review
   also shows that partial batches are skipped (34 patterns per default epoch)
   and output-layer bias gradients are never reset or applied. Review these
   together with configuration validation and the loss/gradient formula.
2. **Make initialization reproducible.** The configured `seed` drives the custom
   shuffle generator, but weight initialization uses `rand()` without `srand()`.
   The custom generator also uses signed arithmetic that can overflow. Define
   reproducible initialization and pattern ordering for all implementations.
3. **Add a Makefile and validation checks.** Provide sequential and backend
   targets, a debug/sanitizer build, and a clean target. Check predictions, loss,
   and gradients against the corrected reference with documented numerical
   tolerances; floating-point reduction order may change parallel results.
4. **Define a benchmark protocol.** Save configurations, compiler flags, commit
   IDs, hardware, resource counts, and repeated timings. The current elapsed
   timer includes dataset loading, training, and testing, but excludes network
   initialization and cleanup. Add separate phase measurements and report
   `speedup = T_seq / T_parallel` and CPU `efficiency = speedup / workers`.
   State the baseline and resource definition explicitly for GPU comparisons.
5. **Prepare delivery infrastructure.** Add backend-specific cluster scripts,
   report outlines, and the confirmed deadlines and accelerator requirements.
   The initial commit already tracks `.DS_Store` files and the supplied `exec`;
   consider untracking these generated files in a separate cleanup commit.

The starter source is unchanged by this scaffold. Local compilation succeeds
with a `RAND_MAX` conversion warning; the sanitizer run fails on the image
reader overflow above. Parallel implementations and cluster performance
measurements have not yet been performed.
