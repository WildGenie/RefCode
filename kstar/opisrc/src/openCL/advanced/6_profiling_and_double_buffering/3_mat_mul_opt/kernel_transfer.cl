__kernel void mat_mul(__global float *M_A,
                      __global float *M_B,
                      __global float *M_C) {
  __global float (*A)[COL_A] = (__global float (*)[COL_A])M_A;
  __global float (*B)[COL_B] = (__global float (*)[COL_B])M_B;
  __global float (*C)[COL_B] = (__global float (*)[COL_B])M_C;

  int i, j, k;
  i = get_global_id(1);
  j = get_global_id(0);
  if (i >= ROW_A || j >= COL_B) return;

  float sum = 0.0f;
  for (k = 0; k < COL_A; k++) {
    sum += A[i][k] * B[k][j];
  }
  C[i][j] = sum;
}

