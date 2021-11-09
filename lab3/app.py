from flask import Flask, request, Response, json
from werkzeug.exceptions import BadRequest, NotFound

from db import Database

app = Flask(__name__)

@app.route("/movies", methods=["GET"])
def get_movies():
    return Response(json.dumps(db.get_movies()), status=200, mimetype='application/json')

@app.route("/movies", methods=["POST"])
def add_movie():
    try:
        db.add_movie(request.get_json())
    except BadRequest:
        return Response(status=400)

    return Response(status=201)

@app.route("/movie/<id>", methods=["PUT"])
def update_movie():
    try:
        db.update_movie(id, request.get_json())
    except BadRequest:
        return Response(status=400)
    except NotFound:
        return Response(status=404)
    
    return Response(status=201)

@app.route("/movie/<id>", methods=["GET"])
def get_movie(id):
    try:
        movie = db.get_movie(id)
    except NotFound:
        return Response(status=404)
    
    return Response(movie.toJSON(), status=201, mimetype='application/json')

@app.route("/movie/<id>", methods=["DELETE"])
def delete_movie(id):
    try:
        db.delete_movie(id)
    except NotFound:
        return Response(status=404)
    
    return Response(status=200)

@app.route("/reset", methods=["DELETE"])
def flush_db():
    db.flush_db()

    return Response(status=200)


if __name__ == "__main__":
    db = Database()

    port = app.config.get("PORT", 5000)
    app.run(host="0.0.0.0", port=port, debug=True)