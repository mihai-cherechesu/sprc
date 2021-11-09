import json

class Object:
    def toJSON(self):
        return json.dumps(self, default=lambda o: o.__dict__, sort_keys=True, indent=4)

class Movie(Object):
    def __init__(self, identifier, name):
        self.id = identifier
        self.name = name