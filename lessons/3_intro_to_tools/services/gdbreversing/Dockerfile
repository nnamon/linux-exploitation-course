FROM ubuntu:latest
ENV user=gdbrev
RUN dpkg --add-architecture i386
RUN sed -i -e 's/archive\.ubuntu\.com/mirror\.0x\.sg/g' /etc/apt/sources.list
RUN apt-get update
RUN apt-get install -y xinetd libc6:i386 libncurses5:i386 libstdc++6:i386
RUN useradd -m $user
RUN echo "$user     hard    nproc       20" >> /etc/security/limits.conf
COPY ./gdbrev /home/$user/gdbrev
COPY ./flag /home/$user/flag
COPY ./gdbrevservice /etc/xinetd.d/gdbrevservice
RUN chown -R root:$user /home/$user
RUN chmod -R 750 /home/$user
RUN chown root:$user /home/$user/flag
RUN chmod 440 /home/$user/flag
EXPOSE 31337
CMD ["/usr/sbin/xinetd", "-d"]
