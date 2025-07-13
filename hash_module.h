#include <stdio.h>

#define TABLE_SIZE 5000003  
#define INDEX_RECORD_SIZE (NAME_BUFFER_SIZE + sizeof(long))
#define FIELD_SIZE 512

enum fields {
    code,
    url,
    creator,
    created_t,
    created_datetime,
    last_modified_t,
    last_modified_datetime,
    last_modified_by,
    last_updated_t,
    last_updated_datetime,
    product_name,
    abbreviated_product_name,
    generic_name,
    quantity,
    packaging,
    packaging_tags,
    packaging_en,
    packaging_text,
    brands,
    brands_tags,
    brands_en,
    categories,
    categories_tags,
    categories_en,
    origins,
    origins_tags,
    origins_en,
    manufacturing_places,
    manufacturing_places_tags,
    labels,
    labels_tags,
    labels_en,
    emb_codes,
    emb_codes_tags,
    first_packaging_code_geo,
    cities,
    cities_tags,
    purchase_places,
    stores,
    countries,
    countries_tags,
    countries_en,
    ingredients_text,
    ingredients_tags,
    ingredients_analysis_tags,
    allergens,
    allergens_en,
    traces,
    traces_tags,
    traces_en,
    serving_size,
    serving_quantity,
    no_nutrition_data,
    additives_n,
    additives,
    additives_tags,
    additives_en,
    nutriscore_score,
    nutriscore_grade,
    nova_group,
    pnns_groups_1,
    pnns_groups_2,
    food_groups,
    food_groups_tags,
    food_groups_en,
    states,
    states_tags,
    states_en,
    brand_owner,
    environmental_score_score,
    environmental_score_grade,
    nutrient_levels_tags,
    product_quantity,
    owner,
    data_quality_errors_tags,
    unique_scans_n,
    popularity_tags,
    completeness,
    last_image_t,
    last_image_datetime,
    main_category,
    main_category_en,
    image_url,
    image_small_url,
    image_ingredients_url,
    image_ingredients_small_url,
    image_nutrition_url,
    image_nutrition_small_url,
    energy_kj_100g,
    energy_kcal_100g,
    energy_100g,
    energy_from_fat_100g,
    fat_100g,
    saturated_fat_100g,
    butyric_acid_100g,
    caproic_acid_100g,
    caprylic_acid_100g,
    capric_acid_100g,
    lauric_acid_100g,
    myristic_acid_100g,
    palmitic_acid_100g,
    stearic_acid_100g,
    arachidic_acid_100g,
    behenic_acid_100g,
    lignoceric_acid_100g,
    cerotic_acid_100g,
    montanic_acid_100g,
    melissic_acid_100g,
    unsaturated_fat_100g,
    monounsaturated_fat_100g,
    omega_9_fat_100g,
    polyunsaturated_fat_100g,
    omega_3_fat_100g,
    omega_6_fat_100g,
    alpha_linolenic_acid_100g,
    eicosapentaenoic_acid_100g,
    docosahexaenoic_acid_100g,
    linoleic_acid_100g,
    arachidonic_acid_100g,
    gamma_linolenic_acid_100g,
    dihomo_gamma_linolenic_acid_100g,
    oleic_acid_100g,
    elaidic_acid_100g,
    gondoic_acid_100g,
    mead_acid_100g,
    erucic_acid_100g,
    nervonic_acid_100g,
    trans_fat_100g,
    cholesterol_100g,
    carbohydrates_100g,
    sugars_100g,
    added_sugars_100g,
    sucrose_100g,
    glucose_100g,
    fructose_100g,
    galactose_100g,
    lactose_100g,
    maltose_100g,
    maltodextrins_100g,
    starch_100g,
    polyols_100g,
    erythritol_100g,
    fiber_100g,
    soluble_fiber_100g,
    insoluble_fiber_100g,
    proteins_100g,
    casein_100g,
    serum_proteins_100g,
    nucleotides_100g,
    salt_100g,
    added_salt_100g,
    sodium_100g,
    alcohol_100g,
    vitamin_a_100g,
    beta_carotene_100g,
    vitamin_d_100g,
    vitamin_e_100g,
    vitamin_k_100g,
    vitamin_c_100g,
    vitamin_b1_100g,
    vitamin_b2_100g,
    vitamin_pp_100g,
    vitamin_b6_100g,
    vitamin_b9_100g,
    folates_100g,
    vitamin_b12_100g,
    biotin_100g,
    pantothenic_acid_100g,
    silica_100g,
    bicarbonate_100g,
    potassium_100g,
    chloride_100g,
    calcium_100g,
    phosphorus_100g,
    iron_100g,
    magnesium_100g,
    zinc_100g,
    copper_100g,
    manganese_100g,
    fluoride_100g,
    selenium_100g,
    chromium_100g,
    molybdenum_100g,
    iodine_100g,
    caffeine_100g,
    taurine_100g,
    ph_100g,
    fruits_vegetables_nuts_100g,
    fruits_vegetables_nuts_dried_100g,
    fruits_vegetables_nuts_estimate_100g,
    fruits_vegetables_nuts_estimate_from_ingredients_100g,
    collagen_meat_protein_ratio_100g,
    cocoa_100g,
    chlorophyl_100g,
    carbon_footprint_100g,
    carbon_footprint_from_meat_or_fish_100g,
    nutrition_score_fr_100g,
    nutrition_score_uk_100g,
    glycemic_index_100g,
    water_hardness_100g,
    choline_100g,
    phylloquinone_100g,
    beta_glucan_100g,
    inositol_100g,
    carnitine_100g,
    sulphate_100g,
    nitrate_100g,
    acidity_100g,
    carbohydrates_total_100g,
    FIELD_COUNT
};

typedef struct Node {
    long ean_code;        // EAN-13 code as long int
    long line_number;     // Line in the CSV file
    struct Node* next;
} Node;

typedef struct {
    Node** buckets;
} HashTable;

unsigned int hash(long code);

typedef struct {
    long ean_code;
    long line_number;
    long next_offset;
} FileNode;

typedef struct {
    long line_number;
    char product_name[FIELD_SIZE];
    char brands[FIELD_SIZE];
    char quantity[FIELD_SIZE];
    char main_category[FIELD_SIZE];
    char packaging[FIELD_SIZE];
    char countries_en[FIELD_SIZE];
    char origins_en[FIELD_SIZE];

    char nutriscore_grade[FIELD_SIZE];
    char nova_group[FIELD_SIZE];
    char additives_n[FIELD_SIZE];
    char additives_tags[FIELD_SIZE];
    char allergens[FIELD_SIZE];
    char traces[FIELD_SIZE];
    char environmental_score_grade[FIELD_SIZE];

    char energy_kcal_100g[FIELD_SIZE];
    char fat_100g[FIELD_SIZE];
    char saturated_fat_100g[FIELD_SIZE];
    char sugars_100g[FIELD_SIZE];
    char fiber_100g[FIELD_SIZE];
    char proteins_100g[FIELD_SIZE];
    char salt_100g[FIELD_SIZE];
} Product;

HashTable* create_table();

void extract_field(const char *line, int field_index, char *field);

void insert(HashTable* table, long ean_code, long line_number);

void free_table(HashTable* table);

void save_table_to_file(HashTable* table, const char* filename);

long file_search(FILE* filename, long ean_code);

void write_code_hashtable(FILE* csv);

void write_main_index(FILE* csv);
