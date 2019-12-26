- dot(normal, light_dir)
- CamMat
- backface culling
- PersMat
- clipping
-  VPMat

world - *CamMat* - cam - *PersMat* - (projection space /homogeneous clip space)- *DIVIDE* - ndc - *VPMat* - screen