unsigned char fragmentShader_glsl[] = {
  0x23, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6f, 0x6e, 0x20, 0x33, 0x33, 0x30,
  0x20, 0x63, 0x6f, 0x72, 0x65, 0x0a, 0x0a, 0x2f, 0x2f, 0x31, 0x2e, 0x42,
  0x65, 0x6c, 0x65, 0x75, 0x63, 0x68, 0x74, 0x75, 0x6e, 0x67, 0x2d, 0x56,
  0x6f, 0x72, 0x62, 0x65, 0x72, 0x65, 0x69, 0x74, 0x75, 0x6e, 0x67, 0x2c,
  0x20, 0x55, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x2d, 0x56, 0x61, 0x72,
  0x69, 0x61, 0x62, 0x6c, 0x65, 0x6e, 0x20, 0x64, 0x65, 0x66, 0x69, 0x6e,
  0x69, 0x65, 0x72, 0x65, 0x6e, 0x0a, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74,
  0x20, 0x4d, 0x61, 0x74, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x50, 0x61, 0x72,
  0x61, 0x6d, 0x73, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63,
  0x33, 0x20, 0x61, 0x6d, 0x62, 0x69, 0x65, 0x6e, 0x74, 0x3b, 0x0a, 0x20,
  0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x64, 0x69, 0x66, 0x66, 0x75,
  0x73, 0x65, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20,
  0x73, 0x70, 0x65, 0x63, 0x75, 0x6c, 0x61, 0x72, 0x3b, 0x0a, 0x20, 0x20,
  0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x73, 0x68, 0x69, 0x6e, 0x69,
  0x6e, 0x65, 0x73, 0x73, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a, 0x0a, 0x73, 0x74,
  0x72, 0x75, 0x63, 0x74, 0x20, 0x4c, 0x69, 0x67, 0x68, 0x74, 0x50, 0x61,
  0x72, 0x61, 0x6d, 0x73, 0x20, 0x7b, 0x0a, 0x20, 0x20, 0x20, 0x76, 0x65,
  0x63, 0x33, 0x20, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x3b,
  0x0a, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x63, 0x6f, 0x6c,
  0x6f, 0x72, 0x3b, 0x0a, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20,
  0x61, 0x6d, 0x62, 0x69, 0x65, 0x6e, 0x74, 0x3b, 0x0a, 0x7d, 0x3b, 0x0a,
  0x0a, 0x75, 0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x4d, 0x61, 0x74,
  0x65, 0x72, 0x69, 0x61, 0x6c, 0x50, 0x61, 0x72, 0x61, 0x6d, 0x73, 0x20,
  0x6d, 0x61, 0x74, 0x3b, 0x20, 0x2f, 0x2f, 0x20, 0x4d, 0x61, 0x74, 0x72,
  0x69, 0x61, 0x6c, 0x65, 0x20, 0x4c, 0x69, 0x63, 0x68, 0x74, 0x0a, 0x75,
  0x6e, 0x69, 0x66, 0x6f, 0x72, 0x6d, 0x20, 0x4c, 0x69, 0x67, 0x68, 0x74,
  0x50, 0x61, 0x72, 0x61, 0x6d, 0x73, 0x20, 0x6c, 0x69, 0x67, 0x68, 0x74,
  0x3b, 0x20, 0x20, 0x2f, 0x2f, 0x20, 0x4c, 0x69, 0x63, 0x68, 0x74, 0x20,
  0x51, 0x75, 0x65, 0x6c, 0x6c, 0x65, 0x0a, 0x0a, 0x2f, 0x2f, 0x32, 0x2e,
  0x49, 0x6e, 0x70, 0x75, 0x74, 0x0a, 0x69, 0x6e, 0x20, 0x76, 0x65, 0x63,
  0x33, 0x20, 0x46, 0x72, 0x61, 0x67, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69,
  0x6f, 0x6e, 0x3b, 0x0a, 0x69, 0x6e, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20,
  0x76, 0x65, 0x72, 0x74, 0x65, 0x78, 0x5a, 0x75, 0x46, 0x72, 0x61, 0x67,
  0x6d, 0x65, 0x6e, 0x74, 0x4e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x3b, 0x0a,
  0x0a, 0x2f, 0x2f, 0x33, 0x2e, 0x4f, 0x75, 0x74, 0x70, 0x75, 0x74, 0x0a,
  0x6f, 0x75, 0x74, 0x20, 0x76, 0x65, 0x63, 0x34, 0x20, 0x46, 0x72, 0x61,
  0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x3b, 0x0a, 0x0a, 0x76, 0x6f, 0x69,
  0x64, 0x20, 0x6d, 0x61, 0x69, 0x6e, 0x28, 0x29, 0x20, 0x7b, 0x0a, 0x0a,
  0x20, 0x20, 0x20, 0x2f, 0x2f, 0x42, 0x65, 0x6c, 0x65, 0x75, 0x63, 0x68,
  0x74, 0x75, 0x6e, 0x67, 0x20, 0x62, 0x65, 0x72, 0x65, 0x63, 0x68, 0x6e,
  0x65, 0x6e, 0x20, 0x75, 0x6e, 0x64, 0x20, 0x53, 0x63, 0x68, 0x61, 0x74,
  0x74, 0x69, 0x65, 0x72, 0x75, 0x6e, 0x20, 0x69, 0x6d, 0x70, 0x6c, 0x65,
  0x6d, 0x65, 0x6e, 0x74, 0x69, 0x65, 0x72, 0x65, 0x6e, 0x0a, 0x0a, 0x20,
  0x20, 0x20, 0x2f, 0x2f, 0x20, 0x20, 0x28, 0x30, 0x29, 0x20, 0x4d, 0x61,
  0x74, 0x72, 0x69, 0x61, 0x6c, 0x20, 0x46, 0x61, 0x72, 0x62, 0x65, 0x0a,
  0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x72, 0x65, 0x73, 0x75,
  0x6c, 0x74, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x76, 0x65,
  0x63, 0x33, 0x28, 0x30, 0x2e, 0x32, 0x2c, 0x20, 0x30, 0x2e, 0x32, 0x2c,
  0x20, 0x30, 0x2e, 0x32, 0x29, 0x3b, 0x0a, 0x0a, 0x20, 0x20, 0x20, 0x2f,
  0x2f, 0x20, 0x20, 0x28, 0x31, 0x29, 0x20, 0x41, 0x6d, 0x62, 0x69, 0x65,
  0x6e, 0x74, 0x0a, 0x20, 0x20, 0x20, 0x72, 0x65, 0x73, 0x75, 0x6c, 0x74,
  0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x3d, 0x20, 0x6d, 0x61, 0x74, 0x2e,
  0x61, 0x6d, 0x62, 0x69, 0x65, 0x6e, 0x74, 0x20, 0x2a, 0x20, 0x6c, 0x69,
  0x67, 0x68, 0x74, 0x2e, 0x61, 0x6d, 0x62, 0x69, 0x65, 0x6e, 0x74, 0x3b,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x20, 0x31, 0x2e,
  0xe7, 0x8e, 0xaf, 0xe5, 0xa2, 0x83, 0xe5, 0x85, 0x89, 0xe7, 0x85, 0xa7,
  0x20, 0x61, 0x6d, 0x62, 0x69, 0x65, 0x6e, 0x74, 0x0a, 0x0a, 0x20, 0x20,
  0x20, 0x2f, 0x2f, 0x20, 0x20, 0x28, 0x32, 0x29, 0x20, 0x44, 0x69, 0x66,
  0x66, 0x75, 0x73, 0x65, 0x0a, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33,
  0x20, 0x4e, 0x20, 0x3d, 0x20, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x69,
  0x7a, 0x65, 0x28, 0x76, 0x65, 0x72, 0x74, 0x65, 0x78, 0x5a, 0x75, 0x46,
  0x72, 0x61, 0x67, 0x6d, 0x65, 0x6e, 0x74, 0x4e, 0x6f, 0x72, 0x6d, 0x61,
  0x6c, 0x29, 0x3b, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x2f, 0x2f, 0x20, 0xe6, 0xb3, 0x95, 0xe7, 0xba, 0xbf, 0xe6,
  0x96, 0xb9, 0xe5, 0x90, 0x91, 0x20, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c,
  0x0a, 0x20, 0x20, 0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x4c, 0x20, 0x3d,
  0x20, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x69, 0x7a, 0x65, 0x28, 0x6c,
  0x69, 0x67, 0x68, 0x74, 0x2e, 0x70, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f,
  0x6e, 0x20, 0x2d, 0x20, 0x46, 0x72, 0x61, 0x67, 0x50, 0x6f, 0x73, 0x69,
  0x74, 0x69, 0x6f, 0x6e, 0x29, 0x3b, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f,
  0x20, 0xe5, 0x85, 0xa5, 0xe5, 0xb0, 0x84, 0xe5, 0x85, 0x89, 0xe6, 0x96,
  0xb9, 0xe5, 0x90, 0x91, 0x20, 0x6c, 0x69, 0x67, 0x68, 0x74, 0x20, 0x3d,
  0x20, 0x51, 0x75, 0x65, 0x6c, 0x6c, 0x65, 0x4c, 0x69, 0x63, 0x68, 0x74,
  0x5f, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x20, 0x2d, 0x20,
  0x76, 0x65, 0x72, 0x74, 0x65, 0x78, 0x5f, 0x70, 0x6f, 0x73, 0x69, 0x74,
  0x69, 0x6f, 0x6e, 0x20, 0x28, 0x76, 0x69, 0x65, 0x77, 0x29, 0x0a, 0x20,
  0x20, 0x20, 0x2f, 0x2f, 0x76, 0x65, 0x63, 0x33, 0x20, 0x4c, 0x20, 0x3d,
  0x20, 0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x69, 0x7a, 0x65, 0x28, 0x76,
  0x65, 0x63, 0x33, 0x28, 0x34, 0x2e, 0x30, 0x2c, 0x20, 0x34, 0x2e, 0x30,
  0x2c, 0x20, 0x30, 0x2e, 0x30, 0x29, 0x20, 0x2d, 0x20, 0x46, 0x72, 0x61,
  0x67, 0x50, 0x6f, 0x73, 0x69, 0x74, 0x69, 0x6f, 0x6e, 0x29, 0x3b, 0x0a,
  0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x64, 0x69, 0x66,
  0x66, 0x20, 0x3d, 0x20, 0x6d, 0x61, 0x78, 0x28, 0x64, 0x6f, 0x74, 0x28,
  0x4e, 0x2c, 0x20, 0x4c, 0x29, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x29, 0x3b,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x20,
  0xe5, 0x85, 0xa5, 0xe5, 0xb0, 0x84, 0xe5, 0x85, 0x89, 0xe8, 0xa7, 0x92,
  0xe5, 0xba, 0xa6, 0x20, 0x28, 0x6c, 0x2c, 0x6e, 0x29, 0x0a, 0x20, 0x20,
  0x20, 0x72, 0x65, 0x73, 0x75, 0x6c, 0x74, 0x43, 0x6f, 0x6c, 0x6f, 0x72,
  0x20, 0x2b, 0x3d, 0x20, 0x6d, 0x61, 0x74, 0x2e, 0x64, 0x69, 0x66, 0x66,
  0x75, 0x73, 0x65, 0x20, 0x2a, 0x20, 0x6c, 0x69, 0x67, 0x68, 0x74, 0x2e,
  0x63, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x2a, 0x20, 0x64, 0x69, 0x66, 0x66,
  0x3b, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x20, 0x32, 0x2e,
  0xe6, 0xbc, 0xab, 0xe5, 0x8f, 0x8d, 0xe5, 0xb0, 0x84, 0xe5, 0x85, 0x89,
  0xe7, 0x85, 0xa7, 0x20, 0x64, 0x69, 0x66, 0x66, 0x75, 0x73, 0x65, 0x0a,
  0x0a, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x20, 0x20, 0x28, 0x33, 0x29, 0x20,
  0x53, 0x70, 0x65, 0x6b, 0x75, 0x6c, 0x61, 0x72, 0x0a, 0x20, 0x20, 0x20,
  0x76, 0x65, 0x63, 0x33, 0x20, 0x61, 0x75, 0x67, 0x65, 0x6e, 0x52, 0x69,
  0x63, 0x68, 0x74, 0x75, 0x6e, 0x67, 0x20, 0x3d, 0x20, 0x76, 0x65, 0x63,
  0x33, 0x28, 0x30, 0x2e, 0x30, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x2c, 0x20,
  0x31, 0x2e, 0x30, 0x29, 0x3b, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x20, 0xe8, 0xa7, 0x86,
  0xe7, 0xba, 0xbf, 0xe6, 0x96, 0xb9, 0xe5, 0x90, 0x91, 0x0a, 0x20, 0x20,
  0x20, 0x76, 0x65, 0x63, 0x33, 0x20, 0x72, 0x65, 0x66, 0x6c, 0x65, 0x63,
  0x74, 0x52, 0x69, 0x63, 0x68, 0x74, 0x75, 0x6e, 0x67, 0x20, 0x3d, 0x20,
  0x6e, 0x6f, 0x72, 0x6d, 0x61, 0x6c, 0x69, 0x7a, 0x65, 0x28, 0x72, 0x65,
  0x66, 0x6c, 0x65, 0x63, 0x74, 0x28, 0x2d, 0x4c, 0x2c, 0x20, 0x4e, 0x29,
  0x29, 0x3b, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2f, 0x2f, 0x20, 0xe5, 0x8f,
  0x8d, 0xe5, 0xb0, 0x84, 0xe5, 0x85, 0x89, 0xe6, 0x96, 0xb9, 0xe5, 0x90,
  0x91, 0x0a, 0x20, 0x20, 0x20, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x20, 0x73,
  0x70, 0x65, 0x63, 0x20, 0x3d, 0x20, 0x70, 0x6f, 0x77, 0x28, 0x6d, 0x61,
  0x78, 0x28, 0x64, 0x6f, 0x74, 0x28, 0x61, 0x75, 0x67, 0x65, 0x6e, 0x52,
  0x69, 0x63, 0x68, 0x74, 0x75, 0x6e, 0x67, 0x2c, 0x20, 0x72, 0x65, 0x66,
  0x6c, 0x65, 0x63, 0x74, 0x52, 0x69, 0x63, 0x68, 0x74, 0x75, 0x6e, 0x67,
  0x29, 0x2c, 0x20, 0x30, 0x2e, 0x30, 0x29, 0x2c, 0x20, 0x6d, 0x61, 0x74,
  0x2e, 0x73, 0x68, 0x69, 0x6e, 0x69, 0x6e, 0x65, 0x73, 0x73, 0x29, 0x3b,
  0x20, 0x2f, 0x2f, 0x20, 0xe5, 0x81, 0x8f, 0xe5, 0xb7, 0xae, 0xe5, 0xa4,
  0xb9, 0xe8, 0x84, 0x9a, 0x20, 0x28, 0x72, 0x2c, 0x61, 0x29, 0xef, 0xbc,
  0x8c, 0x50, 0x68, 0x6f, 0x6e, 0x65, 0x2d, 0x42, 0x65, 0x6c, 0x65, 0x75,
  0x63, 0x68, 0x74, 0x75, 0x6e, 0x67, 0x73, 0x6d, 0x6f, 0x64, 0x65, 0x6c,
  0x6c, 0x65, 0x6e, 0x0a, 0x20, 0x20, 0x20, 0x72, 0x65, 0x73, 0x75, 0x6c,
  0x74, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20, 0x2b, 0x3d, 0x20, 0x6d, 0x61,
  0x74, 0x2e, 0x73, 0x70, 0x65, 0x63, 0x75, 0x6c, 0x61, 0x72, 0x20, 0x2a,
  0x20, 0x6c, 0x69, 0x67, 0x68, 0x74, 0x2e, 0x63, 0x6f, 0x6c, 0x6f, 0x72,
  0x20, 0x2a, 0x20, 0x73, 0x70, 0x65, 0x63, 0x3b, 0x20, 0x0a, 0x0a, 0x20,
  0x20, 0x20, 0x46, 0x72, 0x61, 0x67, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x20,
  0x3d, 0x20, 0x76, 0x65, 0x63, 0x34, 0x28, 0x72, 0x65, 0x73, 0x75, 0x6c,
  0x74, 0x43, 0x6f, 0x6c, 0x6f, 0x72, 0x2c, 0x20, 0x31, 0x2e, 0x30, 0x29,
  0x3b, 0x0a, 0x7d, 0x0a
};
unsigned int fragmentShader_glsl_len = 1552;
