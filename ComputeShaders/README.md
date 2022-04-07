# Compute Shaders

## Notes

No user defined inputs or outputs - changes directly on GPU memory.

Compute shader inputs:
```cpp
gl_NumWorkGroups; // uvec3
gl_WorkGroupID; // uvec3
gl_LocalInvocationID; // uvec3
gl_GlobalInvocationID;
gl_LocalInvocationIndex;
```

Workgroups. Aligned in rows and columns, stacked on top of one another - 3D array.

The workgroup size should be multiple to 32 (NVIDIA) or 64 (AMD).

Multiple threads in workgroups - invocations.

Compute shader does not render by its own - a texture needs to be prepared. It should come with vertex and fragment shaders too.

Shared by invocations variables - statr with `shared`.

### Synchronize memory invocations
```cpp
memoryBarrier();
memoryBarrierAtomicCounter();
memoryBarrierImage();
memoryBarrierBuffer();
memoryBarrierShared();
// Creates visibility for all unsynchronized memory operations
groupMemoryBarrier();
// Barrier for execution synchronization of invocations
barrier();
```

### Atomic operations on invocations
```cpp
atomicAdd()
atomicMin()
atomicMax()
atomicAnd()
atomicOr()
atomicXor()
atomicExchange()
atomicCompSwap()
```

### Invocations voting
```cpp
anyInvocationARB(condtion)
allInvocationsARB(condtion)
// All true or all false
allInvocationsEqualARB(condtion)
```

### Create a compute shader
```cpp
const char* computeShaderSource[] = ...

GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
glShadersource(computeShader, 1, &computeShaderSource, NULL);
glCompileShader(computeShader);

GLuint computeProgram = glCreateProgram();
glAttachShader(computeProgram, computeShader);
glLinkProgram(computeProgram);
```

### Use a compute shader
```cpp
glUseProgram(computeProgram);
glDispatchCompute(10, 20, 5); // Workgroup array dimensions - x, y, z
glMemoryBarrier(GL_ALL_BARRIER_BITS); // Check documentation for specific barrier type
```

### Prepare texture for compute shader to render
```cpp
layout(rgba32f, binding = 0) uniform image2D screen;
```

```cpp
#version 460 core
// local is the number of invocations per workgroup
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

void main()
{
}
```

## Reference
* [Modern OpenGL - Compute Shaders](https://www.youtube.com/watch?v=nF4X9BIUzx0)