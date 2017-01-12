FROM ubuntu:latest
ENV user=event0
RUN dpkg --add-architecture i386
RUN sed -i -e 's/archive\.ubuntu\.com/mirror\.0x\.sg/g' /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y xinetd libc6:i386 libncurses5:i386 libstdc++6:i386
RUN useradd -m $user
RUN echo "$user     hard    nproc       20" >> /etc/security/limits.conf
COPY ./event0 /home/$user/event0
COPY ./event0service /etc/xinetd.d/event0service
RUN chown -R root:$user /home/$user
RUN chmod -R 750 /home/$user
EXPOSE 31337
CMD ["/usr/sbin/xinetd", "-d"]
