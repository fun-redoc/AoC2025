#!/bin/bash

# Check for correct number of arguments
if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <template_file_with_dd> <replacement_string>"
  exit 1
fi

template_file="$1"
replacement="$2"

echo "$template_file" "$replacement"

# Check if the template file exists
if [ ! -f "$template_file" ]; then
  echo "Error: File '$template_file' not found."
  exit 2
fi

# Generate new filename by replacing 'dd' with the replacement string
new_filename="${template_file//dd/$replacement}"

# Copy the file to the new filename
cp "$template_file" "$new_filename"

# Replace all occurrences of 'dd' with the replacement string inside the new file
sed -i "s/dd/$replacement/g" "$new_filename"

# replace timestamp
sed -i "s/<created>/$(date +%Y-%m-%d)/" "$new_filename"

echo "Created '$new_filename' with all 'dd' replaced by '$replacement'."
