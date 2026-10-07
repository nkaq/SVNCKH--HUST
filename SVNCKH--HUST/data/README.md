# Data Storage Policy

Không lưu raw dataset lớn trực tiếp trong Git.

Trong repo:
- `sample/`
- `manifests/`
- `processed_sample/`

Ngoài repo:
- Drive / OneDrive
- Hugging Face Dataset
- Zenodo
- NAS

Mỗi release nên có:
- dataset_version
- fixture_version
- sensor_setup_version
- firmware_commit
- manifest
- checksum
