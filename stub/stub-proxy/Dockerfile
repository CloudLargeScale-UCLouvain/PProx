FROM python:3.7
RUN pip install twisted
ADD src/main.py /

EXPOSE 8080
CMD [ "python", "-u", "./main.py" ]
