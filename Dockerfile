FROM espressif/idf:v5.5

RUN apt-get update && apt-get install -y \
    doxygen \
    lftp \
    && rm -rf /var/lib/apt/lists/*
