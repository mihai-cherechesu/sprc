from json.encoder import JSONEncoder

class MovieEncoder(JSONEncoder):
    def default(self, obj):
        if isinstance(obj, Movie):
            return {
                'name': obj.name
            }
        return super(MovieEncoder, self).default(obj)

class Movie:
    def __init__(self, identifier, name):
        self.id = identifier
        self.name = name