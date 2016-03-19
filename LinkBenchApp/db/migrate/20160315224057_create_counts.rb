class CreateCounts < ActiveRecord::Migration
  def change
    create_table :counts do |t|
      t.integer :link_type
      t.integer :count
      t.integer :time
      t.integer :version

      t.timestamps null: false
    end
  end
end
