#!/bin/bash
while true; do
	# Run binary
	echo "Running capture..."
	./capture_image

	while [ ! -f "image.png" ]; do
		sleep 1  
	done
	echo "Got image..."
	response=$(curl -X POST -F 'file=@image.png' "$UPLOAD_URL/upload")
	prediction=$(echo "$response" | grep -o '"prediction": [0-9]*' | awk '{print $2}')

	# Echo the prediction
	echo "Prediction: $prediction"
	
	#sent command to lights
	echo $prediction > /dev/lightcontrol
	# Add logic based on prediction
	echo "Sent curl..."
	rm -f image.png
	echo "Removed image"
	sleep 30
done
