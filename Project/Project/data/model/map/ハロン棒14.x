xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 72;
 0.00000;1.00000;-0.05000;,
 0.03535;1.00000;-0.03535;,
 0.03535;0.00000;-0.03535;,
 0.00000;0.00000;-0.05000;,
 0.05000;1.00000;0.00000;,
 0.05000;0.00000;0.00000;,
 0.03535;1.00000;0.03535;,
 0.03535;0.00000;0.03535;,
 0.00000;1.00000;0.05000;,
 0.00000;0.00000;0.05000;,
 -0.03535;1.00000;0.03535;,
 -0.03535;0.00000;0.03535;,
 -0.05000;1.00000;0.00000;,
 -0.05000;0.00000;0.00000;,
 -0.03535;1.00000;-0.03535;,
 -0.03535;0.00000;-0.03535;,
 0.00000;1.00000;-0.05000;,
 0.00000;0.00000;-0.05000;,
 0.00000;1.00000;0.00000;,
 0.03535;1.00000;-0.03535;,
 0.00000;0.00000;0.00000;,
 0.03535;0.00000;-0.03535;,
 0.00000;1.39000;0.02500;,
 0.07654;1.37478;0.02500;,
 0.07654;1.37478;-0.02500;,
 0.00000;1.39000;-0.02500;,
 0.14142;1.33142;0.02500;,
 0.14142;1.33142;-0.02500;,
 0.18478;1.26654;0.02500;,
 0.18478;1.26654;-0.02500;,
 0.20000;1.19000;0.02500;,
 0.20000;1.19000;-0.02500;,
 0.18478;1.11346;0.02500;,
 0.18478;1.11346;-0.02500;,
 0.14142;1.04858;0.02500;,
 0.14142;1.04858;-0.02500;,
 0.07654;1.00522;0.02500;,
 0.07654;1.00522;-0.02500;,
 0.00000;0.99000;0.02500;,
 0.00000;0.99000;-0.02500;,
 -0.07654;1.00522;0.02500;,
 -0.07654;1.00522;-0.02500;,
 -0.14142;1.04858;0.02500;,
 -0.14142;1.04858;-0.02500;,
 -0.18478;1.11346;0.02500;,
 -0.18478;1.11346;-0.02500;,
 -0.20000;1.19000;0.02500;,
 -0.20000;1.19000;-0.02500;,
 -0.18478;1.26654;0.02500;,
 -0.18478;1.26654;-0.02500;,
 -0.14142;1.33142;0.02500;,
 -0.14142;1.33142;-0.02500;,
 -0.07654;1.37478;0.02500;,
 -0.07654;1.37478;-0.02500;,
 0.00000;1.19000;0.02500;,
 0.07654;1.37478;0.02500;,
 0.00000;1.39000;0.02500;,
 0.14142;1.33142;0.02500;,
 0.18478;1.26654;0.02500;,
 0.20000;1.19000;0.02500;,
 0.18478;1.11346;0.02500;,
 0.14142;1.04858;0.02500;,
 0.07654;1.00522;0.02500;,
 0.00000;0.99000;0.02500;,
 -0.07654;1.00522;0.02500;,
 -0.14142;1.04858;0.02500;,
 -0.18478;1.11346;0.02500;,
 -0.20000;1.19000;0.02500;,
 -0.18478;1.26654;0.02500;,
 -0.14142;1.33142;0.02500;,
 -0.07654;1.37478;0.02500;,
 0.00000;1.19000;-0.02500;;
 
 72;
 4;0,1,2,3;,
 4;1,4,5,2;,
 4;4,6,7,5;,
 4;6,8,9,7;,
 4;8,10,11,9;,
 4;10,12,13,11;,
 4;12,14,15,13;,
 4;14,16,17,15;,
 3;18,19,16;,
 3;18,4,19;,
 3;18,6,4;,
 3;18,8,6;,
 3;18,10,8;,
 3;18,12,10;,
 3;18,14,12;,
 3;18,16,14;,
 3;20,17,21;,
 3;20,21,5;,
 3;20,5,7;,
 3;20,7,9;,
 3;20,9,11;,
 3;20,11,13;,
 3;20,13,15;,
 3;20,15,17;,
 4;22,23,24,25;,
 4;23,26,27,24;,
 4;26,28,29,27;,
 4;28,30,31,29;,
 4;30,32,33,31;,
 4;32,34,35,33;,
 4;34,36,37,35;,
 4;36,38,39,37;,
 4;38,40,41,39;,
 4;40,42,43,41;,
 4;42,44,45,43;,
 4;44,46,47,45;,
 4;46,48,49,47;,
 4;48,50,51,49;,
 4;50,52,53,51;,
 4;52,22,25,53;,
 3;54,55,56;,
 3;54,57,55;,
 3;54,58,57;,
 3;54,59,58;,
 3;54,60,59;,
 3;54,61,60;,
 3;54,62,61;,
 3;54,63,62;,
 3;54,64,63;,
 3;54,65,64;,
 3;54,66,65;,
 3;54,67,66;,
 3;54,68,67;,
 3;54,69,68;,
 3;54,70,69;,
 3;54,56,70;,
 3;71,25,24;,
 3;71,24,27;,
 3;71,27,29;,
 3;71,29,31;,
 3;71,31,33;,
 3;71,33,35;,
 3;71,35,37;,
 3;71,37,39;,
 3;71,39,41;,
 3;71,41,43;,
 3;71,43,45;,
 3;71,45,47;,
 3;71,47,49;,
 3;71,49,51;,
 3;71,51,53;,
 3;71,53,25;;
 
 MeshMaterialList {
  3;
  72;
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
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1;;
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "ハロン棒の棒.png";
   }
  }
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "ハロン棒の14.png";
   }
  }
  Material {
   0.800000;0.800000;0.800000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
   TextureFilename {
    "ハロン棒の14.png";
   }
  }
 }
 MeshTextureCoords {
  72;
  0.000000;-1.000000;,
  0.125000;-1.000000;,
  0.125000;1.000000;,
  0.000000;1.000000;,
  0.250000;-1.000000;,
  0.250000;1.000000;,
  0.375000;-1.000000;,
  0.375000;1.000000;,
  0.500000;-1.000000;,
  0.500000;1.000000;,
  0.625000;-1.000000;,
  0.625000;1.000000;,
  0.750000;-1.000000;,
  0.750000;1.000000;,
  0.875000;-1.000000;,
  0.875000;1.000000;,
  1.000000;-1.000000;,
  1.000000;1.000000;,
  0.750000;-1.000000;,
  1.125000;-1.000000;,
  0.750000;1.000000;,
  1.125000;1.000000;,
  0.499250;0.024348;,
  0.678578;0.060196;,
  0.678578;0.060196;,
  0.499250;0.024348;,
  0.830602;0.162285;,
  0.830602;0.162285;,
  0.932184;0.315068;,
  0.932184;0.315068;,
  0.967854;0.495291;,
  0.967854;0.495291;,
  0.932184;0.675514;,
  0.932184;0.675514;,
  0.830602;0.828297;,
  0.830602;0.828297;,
  0.678578;0.930385;,
  0.678578;0.930385;,
  0.499250;0.966234;,
  0.499250;0.966234;,
  0.319923;0.930385;,
  0.319923;0.930385;,
  0.167898;0.828297;,
  0.167898;0.828297;,
  0.066317;0.675514;,
  0.066317;0.675514;,
  0.030647;0.495291;,
  0.030647;0.495291;,
  0.066317;0.315068;,
  0.066317;0.315068;,
  0.167898;0.162285;,
  0.167898;0.162285;,
  0.319923;0.060196;,
  0.319923;0.060196;,
  0.508113;0.500000;,
  0.333203;0.075618;,
  0.508113;0.040652;,
  0.184924;0.175193;,
  0.085845;0.324214;,
  0.051053;0.500000;,
  0.085845;0.675785;,
  0.184924;0.824807;,
  0.333203;0.924382;,
  0.508113;0.959348;,
  0.683022;0.924382;,
  0.831302;0.824807;,
  0.930381;0.675785;,
  0.965172;0.500000;,
  0.930381;0.324214;,
  0.831302;0.175193;,
  0.683022;0.075618;,
  0.499250;0.495291;;
 }
}
