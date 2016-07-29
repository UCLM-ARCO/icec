FROM debian:stable
MAINTAINER Oscar Aceña <oscar.acena@uclm.es>
RUN echo "deb http://pike.esi.uclm.es/arco sid main" > /etc/apt/sources.list.d/pike.list
RUN apt-get update
RUN apt-get install -y icec
