FROM grafana/grafana:8.3.4

COPY entrypoint.sh /entrypoint.sh
COPY upstream-entrypoint.sh /upstream-entrypoint.sh

COPY ./config/config.ini /etc/grafana/config.ini
COPY ./dashboards /etc/grafana/dashboards
COPY ./provisioning /etc/grafana/provisioning

ENTRYPOINT [ "/entrypoint.sh" ]