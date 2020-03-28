xof 0303txt 0032

template XSkinMeshHeader {
  <3cf169ce-ff7c-44ab-93c0-f78f62d172e2>
  WORD nMaxSkinWeightsPerVertex;
  WORD nMaxSkinWeightsPerFace;
  WORD nBones;
}

template SkinWeights {
  <6f0d123b-bad2-4167-a0d0-80224f25fabb>
  STRING transformNodeName;
  DWORD nWeights;
  array DWORD vertexIndices[nWeights];
  array float weights[nWeights];
  Matrix4x4 matrixOffset;
}

Frame Root {
  FrameTransformMatrix {
     1.000000, 0.000000, 0.000000, 0.000000,
     0.000000,-0.000000,-1.000000, 0.000000,
     0.000000, 1.000000,-0.000000, 0.000000,
     0.000000, 0.000000, 0.000000, 1.000000;;
  }
  Frame Plane1 {
    FrameTransformMatrix {
       1.000000, 0.000000, 0.000000, 0.000000,
       0.000000, 1.000000, 0.000000, 0.000000,
       0.000000, 0.000000, 1.000000, 0.000000,
       0.000000, 0.000000, 0.000000, 1.000000;;
    }
    Frame Plane1_Bone {
      FrameTransformMatrix {
         1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 0.000000, 1.000000, 0.000000,
         0.000000,-1.000000, 0.000000, 0.000000,
         0.000000, 0.000000, 0.000000, 1.000000;;
      }
    } // End of Plane1_Bone
    Frame Plane {
      FrameTransformMatrix {
         8.200602, 0.000000, 0.000000, 0.000000,
         0.000000, 8.200602, 0.000000, 0.000000,
         0.000000, 0.000000, 8.200602, 0.000000,
         0.000000, 0.000000,-1.125329, 1.000000;;
      }
      Mesh { // Plane mesh
        4;
        -1.000000;-1.000000; 0.000000;,
         1.000000;-1.000000; 0.000000;,
         1.000000; 1.000000; 0.000000;,
        -1.000000; 1.000000; 0.000000;;
        1;
        4;0,1,2,3;;
        MeshNormals { // Plane normals
          1;
           0.000000; 0.000000; 1.000000;;
          1;
          4;0,0,0,0;;
        } // End of Plane normals
        MeshTextureCoords { // Plane UV coordinates
          4;
           0.000000; 1.000000;,
           1.000000; 1.000000;,
           1.000000; 0.000000;,
           0.000000; 0.000000;;
        } // End of Plane UV coordinates
        MeshMaterialList { // Plane material list
          1;
          1;
          0;
          Material Plane1 {
             0.640000; 0.640000; 0.640000; 0.000000;;
             96.078431;
             0.500000; 0.500000; 0.500000;;
             0.000000; 0.000000; 0.000000;;
            TextureFilename {"texFixed.png";}
          }
        } // End of Plane material list
        XSkinMeshHeader {
          1;
          3;
          1;
        }
        SkinWeights {
          "Plane1_Bone";
          4;
          0,
          1,
          2,
          3;
           1.000000,
           1.000000,
           1.000000,
           1.000000;
           8.200602, 0.000000, 0.000000, 0.000000,
           0.000000, 0.000000,-8.200602, 0.000000,
           0.000000, 8.200602, 0.000000, 0.000000,
           0.000000,-1.125329, 0.000000, 1.000000;;
        } // End of Plane1_Bone skin weights
      } // End of Plane mesh
    } // End of Plane
  } // End of Plane1
} // End of Root
