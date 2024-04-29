from flask import Flask, request, jsonify
import os
import tensorflow as tf
from tensorflow.keras.models import load_model
import numpy as np
import cv2


app = Flask(__name__)

model_path = '../MLModel/Fire-64x64-color-v7.1-soft.h5'
model = load_model(model_path)
# Define the endpoint to handle image uploads
@app.route('/upload', methods=['POST'])
def upload_file():
    # Check if the POST request contains a file
    if 'file' not in request.files:
        return jsonify({'error': 'No file uploaded'}), 400

    file = request.files['file']
    
    # Ensure the file is an image (you can add more validation if needed)
    if file.filename == '':
        return jsonify({'error': 'No file'}), 400
    if not file.filename.lower().endswith(('.png', '.jpg', '.jpeg')):
        return jsonify({'error': 'Invalid file format, must be PNG or JPEG'}), 400

    # Save the image file temporarily
    uploads_directory = os.path.join(app.root_path, 'uploads')  # Directory within Flask app
    if not os.path.exists(uploads_directory):
        os.makedirs(uploads_directory)
    image_path = os.path.join(uploads_directory, 'uploaded_image.jpg')
    file.save(image_path)
    
    # Resize the image to the desired dimensions
    resized_image = resize_image(image_path, (64, 64))
    
    prediction = run_machine_learning_model(resized_image)

    return jsonify({'prediction': prediction}), 200

def resize_image(image_path, new_size):
    # Load the image using OpenCV
    image = cv2.imread(image_path)
    # Resize the image to the new size
    resized_image = cv2.resize(image, new_size)
    print(resized_image.shape)
    return resized_image


def run_machine_learning_model(image_file):
    resized_image = cv2.resize(image_file, (64, 64))
    # Normalize pixel values to the range [0, 1]
    normalized_image = resized_image.astype(np.float32) / 255.0
    # Add an additional dimension for the batch
    input_image = np.expand_dims(normalized_image, axis=0)
    return int(tf.round(model.predict(input_image)).numpy()[0][0])


if __name__ == '__main__':
    app.run(debug=True)  # Run the Flask app in debug mode
