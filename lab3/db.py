from werkzeug.exceptions import BadRequest, NotFound
from models import Movie

class Database:
    def __init__(self):
        self.movies = {}
        self._last_movie_key = 0

    # Adds a movie to the dictionary.
    def add_movie(self, payload):
        if payload is None:
            raise BadRequest("Bad request")

        movie_name = payload["nume"]
        if movie_name is None:
            raise BadRequest("Bad request")

        self._last_movie_key += 1
        self.movies[self._last_movie_key] = Movie(self._last_movie_key, movie_name)

        return self._last_movie_key

    # Deletes a movie from the dictionary.
    def delete_movie(self, movie_key):
        if int(movie_key) not in self.movies:
            raise NotFound("Not found")

        del self.movies[int(movie_key)]

    # Fetches a movie from the dictionary and creates a copy.
    # of it.
    def get_movie(self, movie_key):
        movie = self.movies.get(int(movie_key))

        if movie is None:
            raise NotFound("Not found")

        movie_ = Movie(movie.id, movie.name)
        return movie_

    def update_movie(self, movie_key, payload):
        if payload is None:
            raise BadRequest("Bad request")

        movie_name = payload["nume"]
        if movie_name is None:
            raise BadRequest("Bad request")

        movie = self.movies.get(int(movie_key))

        if movie is None:
            raise NotFound("Not found")

        movie_ = Movie(movie.id, payload["nume"])
        self.movies[int(movie_key)] = movie_



    # Get a list of all movies stored in the dictionary.
    def get_movies(self):
        movies = []

        for movie_key, movie in self.movies.items():
            movie_ = Movie(movie.id, movie.name)
            movies.append(movie_)

        return movies

    # Flushes the database.
    def flush_db(self):
        self.movies = {}
        self._last_movie_key = 0