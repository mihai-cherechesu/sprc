FROM influxdb:2.1

COPY entrypoint.sh /entrypoint.sh
COPY upstream-entrypoint.sh /upstream-entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]
CMD ["influxd"]