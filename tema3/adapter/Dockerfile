FROM python:alpine3.15 as builder

COPY requirements.txt /requirements.txt

RUN pip install --user -r /requirements.txt

# Production image.
# Not necessary at the moment as there are not a lot of dependencies.
FROM builder

RUN mkdir -p /adapter/src && \
    apk update && \
    apk upgrade && \
    apk add --update bash

COPY --from=builder /root/.local /root/.local
COPY entrypoint.sh /entrypoint.sh
COPY src/adapter.py /adapter/src

WORKDIR /adapter/src

ENV PATH=/root/.local/bin:$PATH

ENTRYPOINT [ "/bin/bash", "/entrypoint.sh" ]
CMD [ "python3", "adapter.py" ]