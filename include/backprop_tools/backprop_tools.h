#ifndef BACKPROP_TOOLS_BACKPROP_TOOLS_H
#define BACKPROP_TOOLS_BACKPROP_TOOLS_H
//#define BACKPROP_TOOLS_NAMESPACE_WRAPPER test_ns
#ifndef BACKPROP_TOOLS_NAMESPACE_WRAPPER
#define BACKPROP_TOOLS_NAMESPACE_WRAPPER
#define BACKPROP_TOOLS_NAMESPACE_WRAPPER_START
#define BACKPROP_TOOLS_NAMESPACE_WRAPPER_END
#else
#define BACKPROP_TOOLS_NAMESPACE BACKPROP_TOOLS_NAMESPACE_WRAPPER ::backprop_tools
#define BACKPROP_TOOLS_NAMESPACE_WRAPPER_START namespace BACKPROP_TOOLS_NAMESPACE_WRAPPER {
#define BACKPROP_TOOLS_NAMESPACE_WRAPPER_END }
#endif
#endif