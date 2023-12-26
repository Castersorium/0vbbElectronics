#!/bin/bash

# Generate the tree
tree=$(tree)

# Define the start and end markers
start_marker="<!-- TREE START -->"
end_marker="<!-- TREE END -->"

# Create a temporary file
temp_file=$(mktemp)

# Check if the start marker exists in the README.md file
if grep -q "$start_marker" README.md; then
    # If the start marker exists, replace the old tree with the new one
    awk -v var="$tree" '/<!-- TREE START -->/{p=1;print;print "```";print var;print "```"} /<!-- TREE END -->/{p=0} !p' README.md > $temp_file
else
    # If the start marker does not exist, append the new tree with markers
    cp README.md $temp_file
    echo -e "$start_marker\n\`\`\`\n$tree\n\`\`\`\n$end_marker" >> $temp_file
fi

# Replace the README.md file with the temporary file
mv $temp_file README.md
