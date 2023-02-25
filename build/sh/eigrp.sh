docker exec -d -w /home r2 ./NetworkSystem 0 2 2 172.18.0.3 172.19.0.2 0
docker exec -d -w /home r3 ./NetworkSystem 0 2 3 172.19.0.3 0
docker exec -d -w /home r4 ./NetworkSystem 0 2 4 172.20.0.3 172.21.0.2 0
docker exec -d -w /home r5 ./NetworkSystem 0 2 5 172.21.0.3 0
docker exec -d -w /home r6 ./NetworkSystem 0 2 6 172.22.0.3 0
docker exec -it -w /home r1 ./NetworkSystem 0 2 1 172.18.0.2 172.20.0.2 172.22.0.2 172.23.0.2 1