# ML/Data review instructions

Applies to `ml/`.

Block merge if:
- windows from the same recording can appear in both train and test;
- preprocessing is fit using test data;
- labels are inferred from the signal instead of Ground Truth;
- metrics are reported without a reproducible split;
- claimed edge deployment has no model/resource information.

Require random seeds/configuration when meaningful.
