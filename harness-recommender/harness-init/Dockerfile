FROM actionml/harness-cli
COPY init-ur.sh /harness-cli/ur/init-ur.sh
COPY ur.json /harness-cli/ur/ur.json
# Add Tini
ENV TINI_VERSION v0.18.0
ADD https://github.com/krallin/tini/releases/download/${TINI_VERSION}/tini /tini
RUN chmod +x /tini
ENTRYPOINT ["/tini", "--"]

CMD ["/harness-cli/ur/init-ur.sh"]
