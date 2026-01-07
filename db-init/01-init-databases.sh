#!/bin/bash
set -e

LANGUAGES=("python" "cpp" "javascript")
DB_PREFIX="rest_db"
DB_USER="rest_user"
DB_PASSWORD="password123"

echo "Creating shared user..."
mysql -u root -p"$MYSQL_ROOT_PASSWORD" <<EOF
CREATE USER IF NOT EXISTS '${DB_USER}'@'%' IDENTIFIED BY '${DB_PASSWORD}';
EOF

for LANG in "${LANGUAGES[@]}"; do
  DB_NAME="${DB_PREFIX}_${LANG}"

  echo "Initializing database: ${DB_NAME}"

  mysql -u root -p"$MYSQL_ROOT_PASSWORD" <<EOF
CREATE DATABASE IF NOT EXISTS ${DB_NAME};
GRANT ALL PRIVILEGES ON ${DB_NAME}.* TO '${DB_USER}'@'%';
USE ${DB_NAME};

CREATE TABLE IF NOT EXISTS videos (
  id INT NOT NULL,
  name VARCHAR(128) NOT NULL,
  likes INT NOT NULL,
  views INT NOT NULL,
  PRIMARY KEY (id)
);

INSERT INTO videos (id, name, likes, views) VALUES
  (0, "ando's funny video", 10, 121),
  (1, "how to make paper planes", 1420, 42000),
  (2, "awesome flip", 23099, 929299)
ON DUPLICATE KEY UPDATE
  name=VALUES(name),
  likes=VALUES(likes),
  views=VALUES(views);
EOF

done

echo "Flushing privileges..."
mysql -u root -p"$MYSQL_ROOT_PASSWORD" -e "FLUSH PRIVILEGES;"

echo "Database initialization complete."

