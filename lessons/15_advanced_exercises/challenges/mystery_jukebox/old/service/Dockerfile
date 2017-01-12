FROM ubuntu:latest
ENV user=diaperuser
RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get install -y xinetd libc6:i386 libncurses5:i386 libstdc++6:i386 ed
RUN useradd -m $user
RUN echo "$user     hard    nproc       20" >> /etc/security/limits.conf
COPY ./diapers /home/$user/diapers
COPY ./flag /home/$user/flag
COPY ./diaperservice /etc/xinetd.d/diaperservice
RUN chown -R root:$user /home/$user
RUN chmod -R 750 /home/$user
RUN chown root:$user /home/$user/flag
RUN chmod 440 /home/$user/flag
EXPOSE 31337
CMD ["/usr/sbin/xinetd", "-d"]
