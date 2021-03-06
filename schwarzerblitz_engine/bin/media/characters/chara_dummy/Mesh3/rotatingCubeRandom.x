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
  Frame Armature {
    FrameTransformMatrix {
       0.000000,-10.720177, 0.000000, 0.000000,
      10.720177, 0.000000, 0.000000, 0.000000,
       0.000000, 0.000000,10.720177, 0.000000,
      -0.013732, 0.355839, 2.134456, 1.000000;;
    }
    Frame Armature_Wrist_IK_L {
      FrameTransformMatrix {
         0.000000, 1.000000, 0.000000, 0.000000,
        -1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 0.000000, 1.000000, 0.000000,
         0.134269, 3.013148, 0.794121, 1.000000;;
      }
    } // End of Armature_Wrist_IK_L
    Frame Armature_Wrist_IK_R {
      FrameTransformMatrix {
         0.000000, 1.000000, 0.000000, 0.000000,
        -1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 0.000000, 1.000000, 0.000000,
         0.137512,-3.023860, 0.751715, 1.000000;;
      }
    } // End of Armature_Wrist_IK_R
    Frame Armature_Leg_IK_L {
      FrameTransformMatrix {
         0.000000, 1.000000, 0.000000, 0.000000,
        -1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 0.000000, 1.000000, 0.000000,
         0.143220, 0.789051,-4.444445, 1.000000;;
      }
    } // End of Armature_Leg_IK_L
    Frame Armature_Leg_IK_R {
      FrameTransformMatrix {
         0.000000, 1.000000, 0.000000, 0.000000,
        -1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 0.000000, 1.000000, 0.000000,
         0.143220,-0.810131,-4.436768, 1.000000;;
      }
    } // End of Armature_Leg_IK_R
    Frame Armature_Elbow_IK_L {
      FrameTransformMatrix {
         0.000000, 1.000000, 0.000000, 0.000000,
        -1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 0.000000, 1.000000, 0.000000,
        -2.138308, 1.981986, 0.794121, 1.000000;;
      }
    } // End of Armature_Elbow_IK_L
    Frame Armature_Elbow_IK_R {
      FrameTransformMatrix {
         0.000000, 1.000000, 0.000000, 0.000000,
        -1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 0.000000, 1.000000, 0.000000,
        -2.135065,-1.989032, 0.751715, 1.000000;;
      }
    } // End of Armature_Elbow_IK_R
    Frame Armature_Knee_IK_L {
      FrameTransformMatrix {
         0.000000,-0.000000, 1.000000, 0.000000,
         1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 1.000000, 0.000000, 0.000000,
         1.493268, 0.789051,-2.808149, 1.000000;;
      }
    } // End of Armature_Knee_IK_L
    Frame Armature_Knee_IK_R {
      FrameTransformMatrix {
         0.000000,-0.000000, 1.000000, 0.000000,
         1.000000, 0.000000, 0.000000, 0.000000,
         0.000000, 1.000000, 0.000000, 0.000000,
         1.493268,-0.810131,-2.815826, 1.000000;;
      }
    } // End of Armature_Knee_IK_R
    Frame Armature_Hips_001 {
      FrameTransformMatrix {
         0.000000, 1.000000, 0.000000, 0.000000,
         0.000000, 0.000000,-1.000000, 0.000000,
        -1.000000, 0.000000, 0.000000, 0.000000,
         0.143220,-0.000000,-0.208418, 1.000000;;
      }
      Frame Armature_Hips {
        FrameTransformMatrix {
           0.000000, 0.000000,-1.000000, 0.000000,
           0.000000,-1.000000,-0.000000, 0.000000,
          -1.000000,-0.000000,-0.000000, 0.000000,
          -0.000000, 0.704609,-0.000000, 1.000000;;
        }
        Frame Armature_Leg_L {
          FrameTransformMatrix {
             1.000000, 0.000000, 0.000000, 0.000000,
             0.000000,-0.991103,-0.133099, 0.000000,
             0.000000, 0.133099,-0.991122, 0.000000,
             0.000000,-0.025123,-0.326565, 1.000000;;
          }
          Frame Armature_Leg_L_001 {
            FrameTransformMatrix {
               1.000000,-0.000000,-0.000000, 0.000000,
               0.000000, 0.999988,-0.004790, 0.000000,
               0.000000, 0.004790, 0.999989, 0.000000,
               0.000000, 1.801297,-0.000000, 1.000000;;
            }
            Frame Armature_Foot_L {
              FrameTransformMatrix {
                 0.072564,-0.076171,-0.994451, 0.000000,
                 0.809088, 0.587519, 0.014036, 0.000000,
                 0.583190,-0.805617, 0.104262, 0.000000,
                 0.000000, 1.735377, 0.000000, 1.000000;;
              }
              Frame Armature_Foot_L_001 {
                FrameTransformMatrix {
                   0.998575,-0.039753, 0.035614, 0.000000,
                   0.011606, 0.813033, 0.582101, 0.000000,
                  -0.052095,-0.580858, 0.812336, 0.000000,
                   0.000000, 0.412709,-0.000000, 1.000000;;
                }
              } // End of Armature_Foot_L_001
            } // End of Armature_Foot_L
          } // End of Armature_Leg_L_001
        } // End of Armature_Leg_L
        Frame Armature_Leg_R {
          FrameTransformMatrix {
             1.000000, 0.000000, 0.000000, 0.000000,
             0.000000,-0.989760, 0.142741, 0.000000,
             0.000000,-0.142741,-0.989744, 0.000000,
             0.000000,-0.022115, 0.313534, 1.000000;;
          }
          Frame Armature_Leg_R_001 {
            FrameTransformMatrix {
               1.000000,-0.000000,-0.000000, 0.000000,
               0.000000, 0.999989, 0.004790, 0.000000,
               0.000000,-0.004790, 0.999988, 0.000000,
               0.000000, 1.801297,-0.000000, 1.000000;;
            }
            Frame Armature_Foot_R {
              FrameTransformMatrix {
                -0.077157, 0.082517,-0.993598, 0.000000,
                 0.809087, 0.587520,-0.014036, 0.000000,
                 0.582601,-0.804991,-0.112095, 0.000000,
                 0.000000, 1.735377,-0.000000, 1.000000;;
              }
              Frame Armature_Foot_R_001 {
                FrameTransformMatrix {
                   0.990128, 0.137134, 0.029000, 0.000000,
                  -0.129374, 0.814495, 0.565562, 0.000000,
                   0.053937,-0.563731, 0.824196, 0.000000,
                   0.000000, 0.412710,-0.000000, 1.000000;;
                }
              } // End of Armature_Foot_R_001
            } // End of Armature_Foot_R
          } // End of Armature_Leg_R_001
        } // End of Armature_Leg_R
        Frame Armature_Spine {
          FrameTransformMatrix {
             1.000000, 0.000000,-0.000000, 0.000000,
            -0.000000, 1.000000, 0.000000, 0.000000,
             0.000000,-0.000000, 1.000000, 0.000000,
             0.000000, 0.704609, 0.000000, 1.000000;;
          }
          Frame Armature_Torso {
            FrameTransformMatrix {
               1.000000, 0.000000,-0.000000, 0.000000,
               0.000000, 1.000000,-0.000000, 0.000000,
               0.000000, 0.000000, 1.000000, 0.000000,
               0.000000, 0.653441, 0.000000, 1.000000;;
            }
            Frame Armature_Arm_L {
              FrameTransformMatrix {
                 1.000000, 0.000000, 0.000000, 0.000000,
                -0.000000,-0.002345,-0.999997, 0.000000,
                 0.000000, 0.999997,-0.002345, 0.000000,
                -0.008951, 0.351638,-0.939881, 1.000000;;
              }
              Frame Armature_Forearm_L {
                FrameTransformMatrix {
                   1.000000, 0.000000, 0.000000, 0.000000,
                  -0.000000, 0.999997, 0.002345, 0.000000,
                   0.000000,-0.002345, 0.999997, 0.000000,
                  -0.000000, 1.083104, 0.000000, 1.000000;;
                }
                Frame Armature_Wrist_L {
                  FrameTransformMatrix {
                     1.000000,-0.000000, 0.000000, 0.000000,
                     0.000000, 0.965302, 0.261135, 0.000000,
                     0.000000,-0.261135, 0.965302, 0.000000,
                     0.000000, 0.990166, 0.000000, 1.000000;;
                  }
                  Frame Armature_Fingers_L {
                    FrameTransformMatrix {
                       1.000000,-0.000000, 0.000000, 0.000000,
                      -0.000000, 0.983682,-0.179914, 0.000000,
                      -0.000000, 0.179914, 0.983682, 0.000000,
                      -0.041654, 0.178353,-0.006474, 1.000000;;
                    }
                    Frame Armature_Fingers_L_001 {
                      FrameTransformMatrix {
                         1.000000,-0.000000, 0.000000, 0.000000,
                         0.000000, 0.996533,-0.083202, 0.000000,
                         0.000000, 0.083202, 0.996533, 0.000000,
                        -0.000000, 0.278401, 0.000000, 1.000000;;
                      }
                      Frame Armature_Fingers_L_002 {
                        FrameTransformMatrix {
                           1.000000, 0.000000, 0.000000, 0.000000,
                          -0.000000, 1.000000, 0.000000, 0.000000,
                          -0.000000, 0.000000, 1.000000, 0.000000,
                           0.000000, 0.159165, 0.000000, 1.000000;;
                        }
                      } // End of Armature_Fingers_L_002
                    } // End of Armature_Fingers_L_001
                  } // End of Armature_Fingers_L
                  Frame Armature_Thumb_L {
                    FrameTransformMatrix {
                       1.000000,-0.000000, 0.000000, 0.000000,
                      -0.000000, 0.889892,-0.456171, 0.000000,
                      -0.000000, 0.456171, 0.889892, 0.000000,
                       0.069511, 0.084236,-0.086483, 1.000000;;
                    }
                    Frame Armature_Thumb_L_001 {
                      FrameTransformMatrix {
                         1.000000, 0.000000, 0.000000, 0.000000,
                        -0.000000, 0.998048, 0.062453, 0.000000,
                         0.000000,-0.062453, 0.998048, 0.000000,
                        -0.000000, 0.181463,-0.000000, 1.000000;;
                      }
                      Frame Armature_Thumb_L_002 {
                        FrameTransformMatrix {
                           1.000000, 0.000000, 0.000000, 0.000000,
                          -0.000000, 0.989215, 0.146468, 0.000000,
                          -0.000000,-0.146468, 0.989215, 0.000000,
                          -0.000000, 0.127278, 0.000000, 1.000000;;
                        }
                      } // End of Armature_Thumb_L_002
                    } // End of Armature_Thumb_L_001
                  } // End of Armature_Thumb_L
                } // End of Armature_Wrist_L
              } // End of Armature_Forearm_L
            } // End of Armature_Arm_L
            Frame Armature_Arm_R {
              FrameTransformMatrix {
                 0.988813, 0.149154, 0.001232, 0.000000,
                 0.001231,-0.016419, 0.999864, 0.000000,
                 0.149154,-0.988678,-0.016419, 0.000000,
                -0.008260, 0.338412, 0.950836, 1.000000;;
              }
              Frame Armature_Forearm_R {
                FrameTransformMatrix {
                   0.999696,-0.000405,-0.024645, 0.000000,
                   0.000348, 0.999997,-0.002319, 0.000000,
                   0.024645, 0.002310, 0.999694, 0.000000,
                   0.000000, 1.083104, 0.000000, 1.000000;;
                }
                Frame Armature_Wrist_R {
                  FrameTransformMatrix {
                     0.983376, 0.002380, 0.181566, 0.000000,
                     0.045152, 0.965302,-0.257201, 0.000000,
                    -0.175879, 0.261123, 0.949148, 0.000000,
                     0.000000, 0.990166,-0.000000, 1.000000;;
                  }
                  Frame Armature_Fingers_R {
                    FrameTransformMatrix {
                       0.999668,-0.006237, 0.024992, 0.000000,
                       0.001640, 0.983682, 0.179906, 0.000000,
                      -0.025706,-0.179805, 0.983366, 0.000000,
                      -0.041594, 0.178353, 0.006853, 1.000000;;
                    }
                    Frame Armature_Fingers_R_001 {
                      FrameTransformMatrix {
                         0.978369, 0.014388,-0.206368, 0.000000,
                         0.002885, 0.996533, 0.083152, 0.000000,
                         0.206849,-0.081949, 0.974935, 0.000000,
                        -0.000000, 0.278401, 0.000000, 1.000000;;
                      }
                      Frame Armature_Fingers_R_002 {
                        FrameTransformMatrix {
                           1.000000,-0.000000, 0.000104, 0.000000,
                           0.000000, 1.000000, 0.000000, 0.000000,
                          -0.000104,-0.000000, 1.000000, 0.000000,
                           0.000000, 0.159165,-0.000000, 1.000000;;
                        }
                      } // End of Armature_Fingers_R_002
                    } // End of Armature_Fingers_R_001
                  } // End of Armature_Fingers_R
                  Frame Armature_Thumb_R {
                    FrameTransformMatrix {
                       0.999799, 0.005238,-0.019332, 0.000000,
                       0.004158, 0.889892, 0.456152, 0.000000,
                       0.019593,-0.456141, 0.889692, 0.000000,
                       0.070296, 0.084235, 0.085846, 1.000000;;
                    }
                    Frame Armature_Thumb_R_001 {
                      FrameTransformMatrix {
                         0.999929, 0.000025, 0.011886, 0.000000,
                         0.000717, 0.998048,-0.062450, 0.000000,
                        -0.011864, 0.062454, 0.997977, 0.000000,
                         0.000000, 0.181463, 0.000000, 1.000000;;
                      }
                      Frame Armature_Thumb_R_002 {
                        FrameTransformMatrix {
                           0.984889,-0.025308,-0.171327, 0.000000,
                          -0.000059, 0.989216,-0.146467, 0.000000,
                           0.173186, 0.144263, 0.974266, 0.000000,
                          -0.000000, 0.127279,-0.000000, 1.000000;;
                        }
                      } // End of Armature_Thumb_R_002
                    } // End of Armature_Thumb_R_001
                  } // End of Armature_Thumb_R
                } // End of Armature_Wrist_R
              } // End of Armature_Forearm_R
            } // End of Armature_Arm_R
            Frame Armature_Neck {
              FrameTransformMatrix {
                 1.000000, 0.000000, 0.000000, 0.000000,
                 0.000000, 1.000000, 0.000000, 0.000000,
                 0.000000,-0.000000, 1.000000, 0.000000,
                 0.000000, 0.501270, 0.000000, 1.000000;;
              }
              Frame Armature_Head {
                FrameTransformMatrix {
                   1.000000, 0.000000, 0.000000, 0.000000,
                   0.000000, 1.000000, 0.000000, 0.000000,
                   0.000000,-0.000000, 1.000000, 0.000000,
                   0.000000, 0.277489, 0.000000, 1.000000;;
                }
              } // End of Armature_Head
            } // End of Armature_Neck
          } // End of Armature_Torso
        } // End of Armature_Spine
      } // End of Armature_Hips
    } // End of Armature_Hips_001
    Frame Cube {
      FrameTransformMatrix {
        -1.447118, 0.000000, 0.000000, 0.000000,
        -0.000000,-1.447118, 0.000000, 0.000000,
         0.000000, 0.000000, 1.447118, 0.000000,
         0.033193, 0.001281,-0.199106, 1.000000;;
      }
      Mesh { // Cube mesh
        56;
         0.109546; 1.000000;-1.000000;,
         0.109546;-1.000000;-1.000000;,
        -0.109546;-1.000000;-1.000000;,
        -0.109546; 1.000000;-1.000000;,
         0.109546; 1.000000; 1.000000;,
        -0.109546; 1.000000; 1.000000;,
        -0.109546;-1.000000; 1.000000;,
         0.109546;-1.000001; 1.000000;,
         0.109546;-1.000000;-1.000000;,
         0.109546;-1.000001; 1.000000;,
        -0.109546;-1.000000; 1.000000;,
        -0.109546;-1.000000;-1.000000;,
         0.109546; 1.000000; 1.000000;,
         0.109546; 1.000000;-1.000000;,
        -0.109546; 1.000000;-1.000000;,
        -0.109546; 1.000000; 1.000000;,
         0.047637; 0.800000;-0.800000;,
         0.047637; 0.800000; 0.800000;,
         0.047637;-0.800000; 0.800000;,
         0.047637;-0.800000;-0.800000;,
         0.109546; 1.000000; 1.000000;,
         0.047637; 0.800000; 0.800000;,
         0.047637; 0.800000;-0.800000;,
         0.109546; 1.000000;-1.000000;,
         0.109546;-1.000001; 1.000000;,
         0.047637;-0.800000; 0.800000;,
         0.047637; 0.800000; 0.800000;,
         0.109546; 1.000000; 1.000000;,
         0.109546;-1.000000;-1.000000;,
         0.047637;-0.800000;-0.800000;,
         0.047637;-0.800000; 0.800000;,
         0.109546;-1.000001; 1.000000;,
         0.109546; 1.000000;-1.000000;,
         0.047637; 0.800000;-0.800000;,
         0.047637;-0.800000;-0.800000;,
         0.109546;-1.000000;-1.000000;,
        -0.047637;-0.800000;-0.800000;,
        -0.047637;-0.800000; 0.800000;,
        -0.047637; 0.800000; 0.800000;,
        -0.047637; 0.800000;-0.800000;,
        -0.109546;-1.000000; 1.000000;,
        -0.047637;-0.800000; 0.800000;,
        -0.047637;-0.800000;-0.800000;,
        -0.109546;-1.000000;-1.000000;,
        -0.109546; 1.000000; 1.000000;,
        -0.047637; 0.800000; 0.800000;,
        -0.047637;-0.800000; 0.800000;,
        -0.109546;-1.000000; 1.000000;,
        -0.109546; 1.000000;-1.000000;,
        -0.047637; 0.800000;-0.800000;,
        -0.047637; 0.800000; 0.800000;,
        -0.109546; 1.000000; 1.000000;,
        -0.109546;-1.000000;-1.000000;,
        -0.047637;-0.800000;-0.800000;,
        -0.047637; 0.800000;-0.800000;,
        -0.109546; 1.000000;-1.000000;;
        14;
        4;0,1,2,3;,
        4;4,5,6,7;,
        4;8,9,10,11;,
        4;12,13,14,15;,
        4;16,17,18,19;,
        4;20,21,22,23;,
        4;24,25,26,27;,
        4;28,29,30,31;,
        4;32,33,34,35;,
        4;36,37,38,39;,
        4;40,41,42,43;,
        4;44,45,46,47;,
        4;48,49,50,51;,
        4;52,53,54,55;;
        MeshNormals { // Cube normals
          14;
           0.000000; 0.000000;-1.000000;,
           0.000000;-0.000000; 1.000000;,
          -0.000003;-1.000000;-0.000000;,
           0.000002; 1.000000; 0.000000;,
           1.000000;-0.000000; 0.000000;,
           0.955280;-0.295704;-0.000000;,
           0.955280;-0.000000;-0.295703;,
           0.955280; 0.295703; 0.000000;,
           0.955280; 0.000000; 0.295703;,
          -1.000000; 0.000000;-0.000000;,
          -0.955280; 0.295704;-0.000000;,
          -0.955280; 0.000000;-0.295703;,
          -0.955280;-0.295703;-0.000000;,
          -0.955280; 0.000000; 0.295703;;
          14;
          4;0,0,0,0;,
          4;1,1,1,1;,
          4;2,2,2,2;,
          4;3,3,3,3;,
          4;4,4,4,4;,
          4;5,5,5,5;,
          4;6,6,6,6;,
          4;7,7,7,7;,
          4;8,8,8,8;,
          4;9,9,9,9;,
          4;10,10,10,10;,
          4;11,11,11,11;,
          4;12,12,12,12;,
          4;13,13,13,13;;
        } // End of Cube normals
        MeshTextureCoords { // Cube UV coordinates
          56;
           0.750052; 0.722682;,
           0.500177; 0.722680;,
           0.500176; 0.750053;,
           0.750052; 0.750054;,
           0.249950; 0.750147;,
           0.000423; 0.750052;,
           0.250300; 0.750052;,
           0.000073; 0.750147;,
           0.500177; 0.722680;,
           0.000073; 0.750147;,
           0.250300; 0.750052;,
           0.500176; 0.750053;,
           0.999927; 0.722683;,
           0.750052; 0.722682;,
           0.750052; 0.750054;,
           0.999927; 0.750056;,
           0.224122; 0.974078;,
           0.224026; 0.776294;,
           0.026250; 0.776092;,
           0.026346; 0.973876;,
           0.249950; 0.750147;,
           0.224026; 0.776294;,
           0.224122; 0.974078;,
           0.250300; 1.000023;,
           0.000073; 0.750147;,
           0.026250; 0.776092;,
           0.224026; 0.776294;,
           0.249950; 0.750147;,
           0.000423; 1.000023;,
           0.026346; 0.973876;,
           0.026250; 0.776092;,
           0.000073; 0.750147;,
           0.250300; 1.000023;,
           0.224122; 0.974078;,
           0.026346; 0.973876;,
           0.000423; 1.000023;,
           0.224472; 0.973983;,
           0.224376; 0.776199;,
           0.026601; 0.775996;,
           0.026696; 0.973781;,
           0.250300; 0.750052;,
           0.224376; 0.776199;,
           0.224472; 0.973983;,
           0.250650; 0.999927;,
           0.000423; 0.750052;,
           0.026601; 0.775996;,
           0.224376; 0.776199;,
           0.250300; 0.750052;,
           0.000772; 0.999927;,
           0.026696; 0.973781;,
           0.026601; 0.775996;,
           0.000423; 0.750052;,
           0.250650; 0.999927;,
           0.224472; 0.973983;,
           0.026696; 0.973781;,
           0.000772; 0.999927;;
        } // End of Cube UV coordinates
        MeshMaterialList { // Cube material list
          1;
          14;
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0,
          0;
          Material Material {
             0.640000; 0.640000; 0.640000; 1.000000;;
             96.078431;
             0.500000; 0.500000; 0.500000;;
             0.000000; 0.000000; 0.000000;;
            TextureFilename {"mdc.png";}
          }
        } // End of Cube material list
        XSkinMeshHeader {
          1;
          3;
          40;
        }
        SkinWeights {
          "Armature_Arm_L";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.447114,-0.003393, 0.000000,
           0.000000,-0.003393, 1.447114, 0.000000,
          -0.101075,-0.936262,-0.997966, 1.000000;;
        } // End of Armature_Arm_L skin weights
        SkinWeights {
          "Armature_Arm_R";
          0;
          -1.430929,-0.001782,-0.215843, 0.000000,
           0.001782, 1.446921,-0.023760, 0.000000,
           0.215843,-0.023760,-1.430733, 0.000000,
          -0.248351,-0.935981, 0.971870, 1.000000;;
        } // End of Armature_Arm_R skin weights
        SkinWeights {
          "Armature_Elbow_IK_R";
          0;
           0.000000, 1.447118, 0.000000, 0.000000,
          -1.447118, 0.000000, 0.000000, 0.000000,
           0.000000, 0.000000, 1.447118, 0.000000,
           1.990313,-2.168258,-0.950821, 1.000000;;
        } // End of Armature_Elbow_IK_R skin weights
        SkinWeights {
          "Armature_Foot_L";
          0;
          -0.105007,-1.170846,-0.843944, 0.000000,
           1.441306,-0.129266, 0.000003, 0.000000,
          -0.075387,-0.840571, 1.175545, 0.000000,
           0.555465,-2.625331, 3.384476, 1.000000;;
        } // End of Armature_Foot_L skin weights
        SkinWeights {
          "Armature_Hips_001";
          56;
          0,
          1,
          2,
          3,
          4,
          5,
          6,
          7,
          8,
          9,
          10,
          11,
          12,
          13,
          14,
          15,
          16,
          17,
          18,
          19,
          20,
          21,
          22,
          23,
          24,
          25,
          26,
          27,
          28,
          29,
          30,
          31,
          32,
          33,
          34,
          35,
          36,
          37,
          38,
          39,
          40,
          41,
          42,
          43,
          44,
          45,
          46,
          47,
          48,
          49,
          50,
          51,
          52,
          53,
          54,
          55;
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000,
           1.000000;
           0.000000, 0.000000, 1.447118, 0.000000,
          -1.447118,-0.000000, 0.000000, 0.000000,
           0.000000,-1.447118, 0.000000, 0.000000,
           0.001281,-0.009312, 0.110027, 1.000000;;
        } // End of Armature_Hips_001 skin weights
        SkinWeights {
          "Armature_Wrist_L";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.396906, 0.377892, 0.000000,
          -0.000000, 0.377892, 1.396906, 0.000000,
          -0.101075,-3.166728,-0.172263, 1.000000;;
        } // End of Armature_Wrist_L skin weights
        SkinWeights {
          "Armature_Thumb_R_002";
          0;
          -1.425204,-0.001780,-0.250882, 0.000000,
           0.001779, 1.446973,-0.020368, 0.000000,
           0.250882,-0.020368,-1.425059, 0.000000,
          -0.319397,-3.418884, 0.832718, 1.000000;;
        } // End of Armature_Thumb_R_002 skin weights
        SkinWeights {
          "Armature_Thumb_R";
          0;
          -1.447016,-0.001916,-0.017071, 0.000000,
           0.001916, 1.411102,-0.320837, 0.000000,
           0.017071,-0.320838,-1.411000, 0.000000,
          -0.188597,-2.853598, 1.557228, 1.000000;;
        } // End of Armature_Thumb_R skin weights
        SkinWeights {
          "Armature_Knee_IK_L";
          0;
          -0.000000,-1.447118, 0.000000, 0.000000,
           0.000000,-0.000000,-1.447118, 0.000000,
           1.447118, 0.000000, 0.000000, 0.000000,
           2.609043,-1.460075,-0.787770, 1.000000;;
        } // End of Armature_Knee_IK_L skin weights
        SkinWeights {
          "Armature_Wrist_R";
          0;
          -1.447068,-0.001498, 0.011912, 0.000000,
           0.001497, 1.402086, 0.358194, 0.000000,
          -0.011912, 0.358194,-1.402036, 0.000000,
          -0.099618,-3.166462, 0.173269, 1.000000;;
        } // End of Armature_Wrist_R skin weights
        SkinWeights {
          "Armature_Wrist_IK_L";
          0;
           0.000000, 1.447118, 0.000000, 0.000000,
          -1.447118, 0.000000, 0.000000, 0.000000,
           0.000000, 0.000000, 1.447118, 0.000000,
          -3.011867, 0.101075,-0.993228, 1.000000;;
        } // End of Armature_Wrist_IK_L skin weights
        SkinWeights {
          "Armature_Knee_IK_R";
          0;
          -0.000000,-1.447118, 0.000000, 0.000000,
           0.000000,-0.000000,-1.447118, 0.000000,
           1.447118, 0.000000, 0.000000, 0.000000,
           2.616719,-1.460075, 0.811412, 1.000000;;
        } // End of Armature_Knee_IK_R skin weights
        SkinWeights {
          "Armature_Neck";
          0;
          -1.447118,-0.000000,-0.000000, 0.000000,
          -0.000000, 0.000000, 1.447118, 0.000000,
           0.000000, 1.447118, 0.000000, 0.000000,
          -0.110027,-1.145399,-0.001281, 1.000000;;
        } // End of Armature_Neck skin weights
        SkinWeights {
          "Armature_Thumb_L_001";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.431511,-0.211956, 0.000000,
          -0.000000,-0.211956, 1.431511, 0.000000,
          -0.170586,-3.126800,-1.366720, 1.000000;;
        } // End of Armature_Thumb_L_001 skin weights
        SkinWeights {
          "Armature_Head";
          0;
          -1.447118,-0.000000,-0.000000, 0.000000,
          -0.000000, 0.000000, 1.447118, 0.000000,
           0.000000, 1.447118, 0.000000, 0.000000,
          -0.110027,-1.422888,-0.001281, 1.000000;;
        } // End of Armature_Head skin weights
        SkinWeights {
          "Armature_Leg_IK_R";
          0;
           0.000000, 1.447118, 0.000000, 0.000000,
          -1.447118, 0.000000, 0.000000, 0.000000,
           0.000000, 0.000000, 1.447118, 0.000000,
           0.811412, 0.110027, 4.237662, 1.000000;;
        } // End of Armature_Leg_IK_R skin weights
        SkinWeights {
          "Armature_Fingers_L";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.442100, 0.120403, 0.000000,
          -0.000000, 0.120403, 1.442100, 0.000000,
          -0.059421,-3.260670,-0.764910, 1.000000;;
        } // End of Armature_Fingers_L skin weights
        SkinWeights {
          "Armature_Leg_L";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
           0.000000,-0.192606,-1.434216, 0.000000,
          -0.000000,-1.434243, 0.192606, 0.000000,
          -0.110027,-0.775763,-0.224020, 1.000000;;
        } // End of Armature_Leg_L skin weights
        SkinWeights {
          "Armature_Foot_L_001";
          0;
          -0.088370,-1.444417,-0.000000, 0.000000,
           1.444390,-0.088368, 0.000002, 0.000000,
           0.000001,-0.000000, 1.447118, 0.000000,
           0.795977,-0.493474, 4.485065, 1.000000;;
        } // End of Armature_Foot_L_001 skin weights
        SkinWeights {
          "Armature_Torso";
          0;
          -1.447118,-0.000000,-0.000000, 0.000000,
          -0.000000, 0.000000, 1.447118, 0.000000,
           0.000000, 1.447118, 0.000000, 0.000000,
          -0.110027,-0.644129,-0.001281, 1.000000;;
        } // End of Armature_Torso skin weights
        SkinWeights {
          "Armature_Thumb_L";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.415479,-0.300945, 0.000000,
           0.000000,-0.300945, 1.415479, 0.000000,
          -0.170586,-2.853876,-1.559331, 1.000000;;
        } // End of Armature_Thumb_L skin weights
        SkinWeights {
          "Armature_Hips";
          0;
          -1.447118,-0.000000,-0.000000, 0.000000,
          -0.000000,-0.000000, 1.447118, 0.000000,
          -0.000000, 1.447118, 0.000000, 0.000000,
          -0.110027, 0.713921,-0.001281, 1.000000;;
        } // End of Armature_Hips skin weights
        SkinWeights {
          "Armature_Leg_L_001";
          0;
          -1.447118, 0.000000,-0.000001, 0.000000,
           0.000001,-0.185734,-1.435122, 0.000000,
          -0.000000,-1.435149, 0.185734, 0.000000,
          -0.110026,-2.575958,-0.236361, 1.000000;;
        } // End of Armature_Leg_L_001 skin weights
        SkinWeights {
          "Armature_Foot_R_001";
          0;
          -0.074460,-1.444914,-0.028809, 0.000000,
           1.445224,-0.074479, 0.000142, 0.000000,
          -0.001621,-0.028764, 1.446831, 0.000000,
          -0.877354,-0.488479, 4.467210, 1.000000;;
        } // End of Armature_Foot_R_001 skin weights
        SkinWeights {
          "Armature_Leg_R_001";
          0;
          -1.447118, 0.000000,-0.000001, 0.000000,
           0.000001, 0.199703,-1.433295, 0.000000,
           0.000000,-1.433272,-0.199703, 0.000000,
          -0.110027,-2.573715, 0.218862, 1.000000;;
        } // End of Armature_Leg_R_001 skin weights
        SkinWeights {
          "Armature_Elbow_IK_L";
          0;
           0.000000, 1.447118, 0.000000, 0.000000,
          -1.447118, 0.000000, 0.000000, 0.000000,
           0.000000, 0.000000, 1.447118, 0.000000,
          -1.980705,-2.171501,-0.993228, 1.000000;;
        } // End of Armature_Elbow_IK_L skin weights
        SkinWeights {
          "Armature_Thumb_L_002";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.447118, 0.000000, 0.000000,
           0.000000,-0.000000, 1.447118, 0.000000,
          -0.170586,-3.419165,-0.875362, 1.000000;;
        } // End of Armature_Thumb_L_002 skin weights
        SkinWeights {
          "Armature_Leg_IK_L";
          0;
           0.000000, 1.447118, 0.000000, 0.000000,
          -1.447118, 0.000000, 0.000000, 0.000000,
           0.000000, 0.000000, 1.447118, 0.000000,
          -0.787770, 0.110027, 4.245338, 1.000000;;
        } // End of Armature_Leg_IK_L skin weights
        SkinWeights {
          "Armature_Thumb_R_001";
          0;
          -1.447116,-0.001884, 0.000011, 0.000000,
          -0.001862, 1.428385,-0.232082, 0.000000,
           0.000291,-0.232082,-1.428386, 0.000000,
          -0.170152,-3.126520, 1.366762, 1.000000;;
        } // End of Armature_Thumb_R_001 skin weights
        SkinWeights {
          "Armature_Fingers_R";
          0;
          -1.446281,-0.001704, 0.049182, 0.000000,
           0.001703, 1.443651, 0.100095, 0.000000,
          -0.049182, 0.100095,-1.442814, 0.000000,
          -0.032983,-3.260391, 0.766555, 1.000000;;
        } // End of Armature_Fingers_R skin weights
        SkinWeights {
          "Armature_Foot_R";
          0;
           0.111655,-1.170845,-0.843093, 0.000000,
           1.440599, 0.137447,-0.000094, 0.000000,
           0.080155,-0.839274, 1.176157, 0.000000,
          -0.564546,-2.623773, 3.380146, 1.000000;;
        } // End of Armature_Foot_R skin weights
        SkinWeights {
          "Armature_Fingers_R_001";
          0;
          -1.425170,-0.001780,-0.251073, 0.000000,
           0.001781, 1.446973,-0.020367, 0.000000,
           0.251073,-0.020367,-1.425025, 0.000000,
          -0.241377,-3.462877, 1.030518, 1.000000;;
        } // End of Armature_Fingers_R_001 skin weights
        SkinWeights {
          "Armature_Fingers_L_002";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.447118,-0.000000, 0.000000,
           0.000000, 0.000000, 1.447118, 0.000000,
          -0.059421,-3.622323,-1.056716, 1.000000;;
        } // End of Armature_Fingers_L_002 skin weights
        SkinWeights {
          "Armature_Fingers_L_001";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.447118,-0.000000, 0.000000,
          -0.000000, 0.000000, 1.447118, 0.000000,
          -0.059421,-3.463158,-1.056716, 1.000000;;
        } // End of Armature_Fingers_L_001 skin weights
        SkinWeights {
          "Armature_Wrist_IK_R";
          0;
           0.000000, 1.447118, 0.000000, 0.000000,
          -1.447118, 0.000000, 0.000000, 0.000000,
           0.000000, 0.000000, 1.447118, 0.000000,
           3.025141, 0.104318,-0.950821, 1.000000;;
        } // End of Armature_Wrist_IK_R skin weights
        SkinWeights {
          "Armature_Fingers_R_002";
          0;
          -1.425196,-0.001780,-0.250925, 0.000000,
           0.001779, 1.446973,-0.020368, 0.000000,
           0.250925,-0.020368,-1.425051, 0.000000,
          -0.241270,-3.622042, 1.030544, 1.000000;;
        } // End of Armature_Fingers_R_002 skin weights
        SkinWeights {
          "Armature_Forearm_R";
          0;
          -1.425174,-0.001780,-0.251047, 0.000000,
           0.001781, 1.446973,-0.020367, 0.000000,
           0.251047,-0.020367,-1.425030, 0.000000,
          -0.271408,-2.021420, 0.960788, 1.000000;;
        } // End of Armature_Forearm_R skin weights
        SkinWeights {
          "Armature_Forearm_L";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
          -0.000000,-1.447117,-0.000000, 0.000000,
          -0.000000,-0.000000, 1.447118, 0.000000,
          -0.101075,-2.021701,-0.993228, 1.000000;;
        } // End of Armature_Forearm_L skin weights
        SkinWeights {
          "Armature_Spine";
          0;
          -1.447118,-0.000000,-0.000000, 0.000000,
          -0.000000, 0.000000, 1.447118, 0.000000,
           0.000000, 1.447118,-0.000000, 0.000000,
          -0.110027, 0.009312,-0.001281, 1.000000;;
        } // End of Armature_Spine skin weights
        SkinWeights {
          "Armature_Leg_R";
          0;
          -1.447118, 0.000000,-0.000000, 0.000000,
           0.000000, 0.206566,-1.432323, 0.000000,
           0.000000,-1.432299,-0.206566, 0.000000,
          -0.110027,-0.773437, 0.206532, 1.000000;;
        } // End of Armature_Leg_R skin weights
      } // End of Cube mesh
    } // End of Cube
  } // End of Armature
} // End of Root
