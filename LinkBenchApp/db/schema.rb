# encoding: UTF-8
# This file is auto-generated from the current state of the database. Instead
# of editing this file, please use the migrations feature of Active Record to
# incrementally modify your database, and then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your
# database schema. If you need to create the application database on another
# system, you should be using db:schema:load, not running all the migrations
# from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended that you check this file into your version control system.

ActiveRecord::Schema.define(version: 20160317220510) do

  create_table "counts", force: :cascade do |t|
    t.integer  "link_type"
    t.integer  "time"
    t.integer  "version"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.integer  "countval"
    t.integer  "node_id"
  end

  add_index "counts", ["node_id"], name: "index_counts_on_node_id"

  create_table "links", force: :cascade do |t|
    t.integer  "id2"
    t.integer  "linktype"
    t.integer  "visibility"
    t.string   "data"
    t.integer  "time"
    t.integer  "version"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.integer  "node_id"
  end

  add_index "links", ["node_id"], name: "index_links_on_node_id"

  create_table "nodes", force: :cascade do |t|
    t.integer  "version"
    t.integer  "time"
    t.string   "data"
    t.datetime "created_at", null: false
    t.datetime "updated_at", null: false
    t.integer  "tnode"
  end

end
