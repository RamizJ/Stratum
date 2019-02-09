// dynamic vars

 name "new" ret "HANDLE"    out                        700
 name "delete" arg "HANDLE" out                        701
 name "vClearAll" out                                  702
 name "vInsert" arg "HANDLE","STRING" ret "FLOAT" out  703
 name "vDelete" arg "HANDLE","FLOAT" ret "FLOAT"  out  704
 name "vGetCount" arg "HANDLE" ret "FLOAT" out         705
 name "vGetType" arg "HANDLE","FLOAT" ret "STRING" out 706

 name "vGetF" arg "HANDLE","FLOAT","STRING" ret "FLOAT"     out 707
 name "vGetS" arg "HANDLE","FLOAT","STRING" ret "STRING"    out 708
 name "vGetH" arg "HANDLE","FLOAT","STRING" ret "HANDLE"    out 709
 name "vSet" arg "HANDLE","FLOAT","STRING","FLOAT" out 710
 name "vSet" arg "HANDLE","FLOAT","STRING","STRING" out 711
 name "vSet" arg "HANDLE","FLOAT","STRING","HANDLE" out 712

//порядок описания этой функции должен быть именно таким !!!
 name "vSort" arg "HANDLE","FLOAT",["STRING"] ret "FLOAT" out 873
 name "vSort" arg "HANDLE",["STRING"] ret "FLOAT" out 874
 name "vSort" arg "HANDLE",["STRING","FLOAT"]  ret "FLOAT" out 875

 name "vSave" arg "HANDLE","HANDLE" ret "FLOAT" out   752
 name "vLoad"  arg "HANDLE" ret "HANDLE" out 753

