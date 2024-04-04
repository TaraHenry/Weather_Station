"""
Flask Documentation:     https://flask.palletsprojects.com/
Jinja2 Documentation:    https://jinja.palletsprojects.com/
Werkzeug Documentation:  https://werkzeug.palletsprojects.com/
This file creates your application.
"""

# from crypt import methods
import site 

from app import app, Config,  mongo, Mqtt
from flask import escape, render_template, request, jsonify, send_file, redirect, make_response, send_from_directory 
from json import dumps, loads 
from werkzeug.utils import secure_filename
from datetime import datetime,timedelta, timezone
from os import getcwd
from os.path import join, exists
from time import time, ctime
from math import floor
 



#####################################
#   Routing for your application    #
#####################################

@app.route('/api/weather/get/<start>/<end>', methods=['GET']) 
def get_all(start,end):   
    '''RETURNS ALL THE DATA FROM THE DATABASE THAT EXIST IN BETWEEN THE START AND END TIMESTAMPS'''
    start = int(start)
    end = int(end)
    if request.method == "GET":
        '''Add your code here to complete this route'''
        try:
            item = mongo.getAll(start,end)
            # data = list(item)
            # print()
            if item:
                return jsonify({"status":"found","data": item})
        except Exception as e:
            msg = str(e)
            print(f"get_all error: f{str(e)}") 
    # FILE DATA NOT EXIST
    return jsonify({"status":"not found","data":[]})
   
@app.route('/api/update', methods=['POST']) 
def update_weather():      
    if request.method == "POST":
        try:
            jsonDoc= request.get_json()
            # Update the document in the 'code' collection with the new passcode

            timestamp = datetime.now().timestamp()
            timestamp = floor(timestamp)
            jsonDoc['timestamp'] = timestamp

            Mqtt.publish("620154033",mongo.dumps(jsonDoc))
            Mqtt.publish("620154033_pub",mongo.dumps(jsonDoc))
            Mqtt.publish("620154033_sub",mongo.dumps(jsonDoc))

            print(f"MQTT: {jsonDoc}")

            item = mongo.addUpdate(jsonDoc)
            if item:
                return jsonify({"status": "complete", "data": "complete"})
        except Exception as e:
            msg = str(e)
            print(f"update Error: {msg}")
        return jsonify({"status": "failed", "data": "failed"})

# @app.route('/api/avg/<start>/<end>', methods=['GET'])
# def get_average_temp(start, end):
#     '''Returns the average of the 'temp' field/variable, using all documents found between specified start and end timestamps'''
#     if request.method == 'GET':
#         try:
#             average = mongo.calculate_avg(start, end)
#             if average:
#                 return jsonify({"status": "complete", "data": average})
#         except Exception as e:
#             return jsonify({"status": "failed", "data": 0})

@app.route('/api/file/get/<filename>', methods=['GET']) 
def get_images(filename):   
    '''RETURNS REQUESTED FILE FROM UPLOADS FOLDER'''
   
    if request.method == "GET":
        '''Add your code here to complete this route'''
        # FILE DOES NOT EXIST
    return jsonify({"status":"file not found"}), 404



@app.route('/api/file/upload',methods=["POST"])  
def upload():
    '''SAVES A FILE TO THE UPLOADS FOLDER'''
    
    if request.method == "POST": 
        file     = request.files['file']
        filename = secure_filename(file.filename)
        file.save(join(getcwd(),Config.UPLOADS_FOLDER , filename))
        return jsonify({"status":"File upload successful", "filename":f"{filename}" })

 


###############################################################
# The functions below should be applicable to all Flask apps. #
###############################################################


@app.route('/<file_name>.txt')
def send_text_file(file_name):
    """Send your static text file."""
    file_dot_text = file_name + '.txt'
    return app.send_static_file(file_dot_text)

@app.after_request
def add_header(response):
    """
    Add headers to both force latest IE rendering engine or Chrome Frame,
    and also tell the browser not to cache the rendered page. If we wanted
    to we could change max-age to 600 seconds which would be 10 minutes.
    """
    response.headers['X-UA-Compatible'] = 'IE=Edge,chrome=1'
    response.headers['Cache-Control'] = 'public, max-age=0'
    return response

@app.errorhandler(405)
def page_not_found(error):
    """Custom 404 page."""    
    return jsonify({"status": 404}), 404



