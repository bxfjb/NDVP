FROM ubuntu:18.04 
# sudo docker run -itd 
# apt update | apt install net-tools
# copy NDVP file and run it
COPY ../build/NetworkSystem /home/
RUN apt update \
    && apt install -y net-tools
