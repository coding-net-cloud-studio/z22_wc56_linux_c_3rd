#!/bin/bash

input_file="b22_需要exclude的文件_编辑.txt"
output_file="o22_from_b22_增加以后.txt"

while IFS= read -r line
do
  echo "${line}\": true," >> "$output_file"
done < "$input_file"
