use sqlx::MySqlPool;
use std::time::Duration;
use tokio::time::sleep;

pub async fn create_pool(
    database_url: &str,
) -> Result<MySqlPool, sqlx::Error> {
    loop {
        match MySqlPool::connect(database_url).await {
            Ok(pool) => {
                tracing::info!("Connected to MySQL");
                return Ok(pool);
            }
            Err(e) => {
                tracing::warn!(
                    "Failed to connect to MySQL: {}. Retrying...",
                    e
                );
                sleep(Duration::from_secs(2)).await;
            }
        }
    }
}
