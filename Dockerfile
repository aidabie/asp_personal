# Stage 1: V8 Builder
FROM ubuntu:22.04 as v8builder
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    build-essential curl git python3 python3-pip python-is-python3 \
    ninja-build nodejs npm cmake pkg-config gnupg lsb-release unzip wget \
    && rm -rf /var/lib/apt/lists/*
# Install depot_tools
RUN git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git /opt/depot_tools
ENV PATH="/opt/depot_tools:${PATH}"

# Fetch and build V8
WORKDIR /opt
RUN fetch v8
WORKDIR /opt/v8
RUN git checkout branch-heads/13.1
RUN gclient sync --with_branch_heads

# Generate build config and build
RUN ./tools/dev/v8gen.py -vv x64.release.sample 
RUN ninja -C out.gn/x64.release.sample v8_monolith -j $(nproc)

# Stage 2: Dev Environment
FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
ARG USERNAME=asp
ARG USER_UID=2000
ARG USER_GID=2000
RUN apt-get update && apt-get install -y \
    sudo openssh-server build-essential curl git \
    python3 python3-pip python-is-python3 vim \
    && rm -rf /var/lib/apt/lists/*

# SSH setup
RUN mkdir /var/run/sshd && \
    echo 'PasswordAuthentication yes' >> /etc/ssh/sshd_config
# Create user
RUN if ! getent group $USER_GID >/dev/null; then \
        groupadd --gid $USER_GID $USERNAME; \
    fi && \
    useradd --uid $USER_UID --gid $USER_GID -m $USERNAME && \
    echo "$USERNAME ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers.d/$USERNAME && \
    chmod 0440 /etc/sudoers.d/$USERNAME && \
    mkdir -p /home/$USERNAME/.ssh && \
    chown -R $USERNAME:$USER_GID /home/$USERNAME/.ssh

# Copy V8 from the builder
COPY --from=v8builder --chown=$USERNAME:$USER_GID /opt/v8 /home/$USERNAME/v8

# Switch to user
USER $USERNAME
WORKDIR /home/$USERNAME/project
RUN ssh-keygen -t ed25519 -f ~/.ssh/id_ed25519

# add ASP things (by joa)
RUN curl -qsL 'https://install.pwndbg.re' | sh -s -- -t pwndbg-gdb
RUN echo "alias gdb=pwndbg" >> ~/.bashrc

# Reset user to root
USER root
EXPOSE 22
RUN apt-get update && apt-get install -y systemtap-sdt-dev cmake nano figlet
RUN apt-get install -y netcat strace ltrace nodejs python3-requests python3-jsonschema
RUN echo "alias gdb=pwndbg" >> ~/.bashrc


# CMD ["/usr/sbin/sshd","-D"]
CMD [ "bash" ]
