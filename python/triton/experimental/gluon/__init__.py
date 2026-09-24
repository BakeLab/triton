from ._runtime import GluonASTSource, GluonJITFunction, constexpr_function, jit
from triton import must_use_result, aggregate

try:
    from . import nvidia
except ImportError:
    # NVIDIA-specific gluon modules are omitted from backend-specific wheels.
    nvidia = None

try:
    from . import amd
except ImportError:
    # AMD-specific gluon modules are omitted from backend-specific wheels.
    amd = None

__all__ = [
    "aggregate", "constexpr_function", "GluonASTSource", "GluonJITFunction", "jit", "must_use_result"
]
if nvidia is not None:
    __all__.append("nvidia")
if amd is not None:
    __all__.append("amd")
