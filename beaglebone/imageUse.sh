#!/bin/bash

while true; do
	# Run binary
	echo "Running capture..."
	./capture_image

	while [ ! -f "image.png" ]; do
		sleep 1  
	done
	echo "Got image..."
	curl -X POST -F 'file=@image.png' "$UPLOAD_URL/upload"

	# Add logic based on prediction
	echo "Sent curl..."
	rm -f image.png
	echo "Removed image"
	sleep 60
done
