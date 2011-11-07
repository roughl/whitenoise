#/bin/bash
find ./  -type f -name "*.[Bb][Mm][Pp]" | while read name; 
do
	new_name=$(echo -n "${name%.*}");
	convert "$name" "$new_name".jpg;
	rm -f "$name"
done
